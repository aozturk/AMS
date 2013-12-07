#include "AMS/IService.h"
#include "AMS/Discovery/Discovery.h"
#include "AMS/Config/Parser.h"

using namespace AMS;

IService* IService::s_instance = NULL;
Poco::FastMutex IService::s_mutex;

using namespace AMS;

IService::IService()
    : m_dispatcher(1),
    m_activity(this, &IService::run_activity),
    m_subsocket(0),
    m_pubsocket(0),
    m_domainId(0),
    m_initialised(false) {

        create_logger();      

        m_hostIP = Configurator::resolveHostIP();
        
        // creates sub socket without message subscriptions
        if (!m_subsocket) {
            m_subsocket = new Socket(m_context, SUB);
            // set poll id returned by poller
            m_pollId = m_poller.add(*m_subsocket, POLL_IN);                
        }
}

IService::~IService() {
    reactor_stop();

    if (m_subsocket) {
        delete m_subsocket;
        m_subsocket = NULL;
    }
    if (m_pubsocket) {
        delete m_pubsocket;
        m_pubsocket = NULL;
    }

    Pubsub_t::iterator iter = m_publishers.begin();
    while (iter != m_publishers.end()) {
        delete iter->second;
        ++iter;
    }
    iter = m_subscribers.begin();
    while (iter != m_subscribers.end()) {
        delete iter->second;
        ++iter;
    }

    m_consoleLogger->getChannel()->close();
    Poco::Logger::destroy("AMSLogger");
}

IService& IService::instance()
{
    Poco::FastMutex::ScopedLock lock(s_mutex);
    if (!s_instance) s_instance = new IService;
    return *s_instance;
}

void IService::destroy()
{
    Poco::FastMutex::ScopedLock lock(s_mutex);
    if (s_instance != NULL) {
        delete s_instance;
        s_instance = NULL;
    }
}

void IService::create_logger() {
    Poco::FormattingChannel* pFCConsole = new  Poco::FormattingChannel(
        new Poco::PatternFormatter("%H:%M:%S.%c [pid:%P tid:%I] %q AMS:  %t"));
    pFCConsole->setChannel(new Poco::ConsoleChannel);
    pFCConsole->open();
    m_consoleLogger = &Poco::Logger::create("AMSLogger", pFCConsole, Poco::Message::PRIO_INFORMATION);
}

void IService::create_domain(std::string domainName, std::string selfDesc) { 
    if (!m_initialised) {
        m_initialised = true;

        // calculate hash value (domain-id) from domain name
        unsigned long hash = 7;
        for (int i=0; i < domainName.size(); ++i)
            hash = ((hash << 5) + hash) + domainName.at(i);
        m_domainId = hash % 230; 

        poco_information_f3(logger(), "created domain: %s:%d on (%s)", domainName, m_domainId, m_hostIP);        
                
        // handle static peers
        m_staticPeers = AMS::parse();

        for (int i=0; i < m_staticPeers.size(); ++i) {
            Heartbeat hbeat;
            hbeat.host = m_staticPeers[i];
            int port = Configurator::getPort(m_domainId);
            for (int j=0; j < MAX_APP_ON_SAME_HOST; ++j) {
                hbeat.pubs[port+j] = true;
            }
            handle_new_peer(hbeat);
        }        

        // starts dynamic discovery
        m_discovery.addDesc(selfDesc);
        register_discovery(&m_aliveness);
        m_discovery.startDiscovery(m_domainId);    
        m_discovery.selfNotify();
    }
}

void IService::reactor_start() {
    m_activity.start();    
}

void IService::reactor_stop() {
    m_discovery.stopDiscovery();

    m_activity.stop(); // request stop
    m_activity.wait(); // wait until activity actually stops
}

void IService::register_discovery(IPeerNotification* notifier) {
    m_discovery.registerDiscovery(notifier);
}

void IService::run_activity() {
    while (!m_activity.isStopped()) {
        int rc = m_poller.poll(POLL_1_SEC);

        if (m_activity.isStopped()) break;

        Poco::FastMutex::ScopedLock lock(m_mutex);

        if (m_poller.hasInput(m_pollId)) {
            SingleMessage message;
            m_subsocket->receive(message);

            char msgId_str [IPubSub::MSG_NAME_SIZE] = {0};
            memcpy(msgId_str, message.raw_data(), IPubSub::MSG_NAME_SIZE);
            int msgId = atoi(strtok(msgId_str, "-"));

            IMsgObj* msg_obj = m_subscribers[msgId]->receive_message(message);
            if (msg_obj->getMessageObjectId() != HB_MSG_ID) {
                poco_debug_f1(logger(), "polled: %d", msg_obj->getMessageObjectId());
            }
            m_dispatcher.dispatch(msg_obj);
        }
    }
}

void IService::handle_new_peer(const Heartbeat& hbeat) {
    Poco::FastMutex::ScopedLock lock(m_mutex);

    for(std::map<int, bool>::const_iterator iter = hbeat.pubs.begin(); iter != hbeat.pubs.end(); ++iter) {
        int peerPort = iter->first;
        if (!m_aliveness.IsPubConnected(hbeat.host, peerPort)) {
            m_subsocket->connect(TCP, hbeat.host, peerPort);
            m_aliveness.AddConnectedPub(hbeat.host, peerPort);
            poco_information_f2(logger(), "connected to: %s:%d", hbeat.host, peerPort);
        }
    }        
}

void IService::send_message(IMsgObj& obj) {
    int messageId = obj.getMessageObjectId();
    Poco::FastMutex::ScopedLock lock(m_mutex);
    if (m_publishers.find(messageId) != m_publishers.end()) {
        IPubSub* pubsub = m_publishers[messageId];
        pubsub->send_message(obj);
        if (messageId != HB_MSG_ID) { // discard logging for heartbeat
            poco_debug_f1(IService::instance().logger(), "sent: %d", messageId);
        }
    } else {
        poco_warning_f1(IService::instance().logger(), "Sent failed! No publisher for message id: %d", messageId);
    }
}

void IService::create_single_publisher() {
    m_pubsocket = new Socket(m_context, PUB);

    int port = m_pubsocket->bind(TCP, m_hostIP, Configurator::getPort(m_domainId));
    m_discovery.addPub(port);
    
    poco_information_f2(logger(), "publisher created: %s:%d", m_hostIP, port);              

    // connect to own publisher
    if (!m_aliveness.IsPubConnected(m_hostIP, port)) {
        m_subsocket->connect(TCP, m_hostIP, port);
        poco_information_f2(logger(), "connected to itself: %s:%d", m_hostIP, port);
        m_aliveness.AddConnectedPub(m_hostIP, port);
    }
}

std::string IService::get_host_ip() const {
    return m_hostIP;
}

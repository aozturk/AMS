#include "AMS/Discovery/ReceiverTask.h"
#include "AMS/IService.h"

using namespace AMS;

ReceiverTask::ReceiverTask() :
Poco::Task("ReceiverTask"), m_selfrcv(false), m_hb_handler(NULL)
{    
}

ReceiverTask::~ReceiverTask()
{
}

void ReceiverTask::initialize(const std::vector<std::string>& args, const Poco::UUID& uuid)
{
    m_family = Poco::Net::IPAddress(args[0]).family();
    m_port = Poco::NumberParser::parseUnsigned(args[1]);
    m_multicastSocket = Poco::Net::MulticastSocket(m_family);
    m_socketAddres = Poco::Net::SocketAddress(args[0], m_port);
    m_if = findInterface(m_family);
    m_self = uuid;

    m_multicastSocket.bind(Poco::Net::SocketAddress(
        Poco::Net::IPAddress(m_family), m_socketAddres.port()), true);
    m_multicastSocket.joinGroup(m_socketAddres.host(), m_if);

    IService::instance().create_subscriber<Heartbeat>();
    m_hb_handler = new HeartbeatHandler(this);
    IService::instance().subscribe<Heartbeat>(*m_hb_handler);
}

void ReceiverTask::destroy() {

    IService::instance().unsubscribe<Heartbeat>();
    if (m_hb_handler) {
        delete m_hb_handler;
        m_hb_handler = 0;
    }
    this->cancel();

    m_multicastSocket.leaveGroup(m_socketAddres.host(), m_if);
        
    m_multicastSocket.close();
}

void ReceiverTask::runTask()
{
    Poco::Logger& logger = IService::instance().logger();
    logger.information(Poco::format("discovery receiver running: %s", m_socketAddres.toString()));

    Poco::Timespan span(KWaitUSec);

    while(!isCancelled())
    {
        if(m_multicastSocket.poll(span, Poco::Net::Socket::SELECT_READ))
        {
            if (isCancelled()) break;
            try
            {
                Poco::Net::SocketAddress sender;
                int nbytes = m_multicastSocket.receiveFrom(m_buffer, sizeof(m_buffer), sender);
                if(0 != nbytes)
                {
                    msgpack::unpacked msg;
                    msgpack::unpack(&msg, m_buffer, nbytes);
                    msgpack::object obj = msg.get();

                    Heartbeat hbeat;
                    obj.convert(&hbeat);

                    handleBeacon(hbeat);
                }
                //	m_multicastSocket.sendTo(buffer, n, sender);	// for echo back
            }
            catch(Poco::Exception& exc)
            {
                logger.error(Poco::format("ReceiverTask: %s", exc.displayText()));
            }
        }
    }
}

void ReceiverTask::handleBeacon(const Heartbeat& hbeat) {
    Poco::FastMutex::ScopedLock lock(m_mutex);

    Poco::UUID uuid;
    bool ret = uuid.tryParse(hbeat.uuid);
    if (ret) {
        Poco::Timestamp ts; //current time
        Poco::Timestamp::TimeVal diff = ts - m_ts;

        if (uuid != m_self) { // not self heartbeat
            if (m_peers.find(uuid) == m_peers.end()) { //not exists
                m_peers[uuid] = hbeat;
                for (int i=0; i<m_notifiers.size(); i++) { m_notifiers.at(i)->OnAlive(hbeat); }
            }
            else if (m_peers[uuid].pubs != hbeat.pubs) {
                m_peers[uuid] = hbeat;
                for (int i=0; i<m_notifiers.size(); i++) { m_notifiers.at(i)->OnUpdate(hbeat); }
            }
            
            m_peers[uuid].expires_at = (unsigned long) (diff + EXPIRY_TIME);
        }

        // delete expired peers
        PeersMap::const_iterator iter = m_peers.begin();
        while (iter != m_peers.end()) {
            const Heartbeat& cur_peer = (*iter).second;
            if (cur_peer.expires_at < diff) {
                for (int i=0; i<m_notifiers.size(); i++) { m_notifiers.at(i)->OnDead(cur_peer); }
                iter = m_peers.erase(iter);
                continue;
            }
            ++iter;
        }
    }
    else {
        IService::instance().logger().error(Poco::format("Unexpected uuid: %s", hbeat.uuid));
    }
}

void ReceiverTask::addNotifier(IPeerNotification* notifier) {
    Poco::FastMutex::ScopedLock lock(m_mutex);

    m_notifiers.push_back(notifier);
}

void ReceiverTask::selfNotify(const Heartbeat& hbeat) {
    Poco::FastMutex::ScopedLock lock(m_mutex);

    for (int i=0; i<m_notifiers.size(); i++) { m_notifiers.at(i)->OnAlive(hbeat); }
}
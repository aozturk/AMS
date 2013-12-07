// Author: Abdullah OZTURK
// Date  : 11.2012
#pragma once

#include "AMS/Net/Context.h"
#include "AMS/PubSub/PubSub.h"
#include "AMS/Reactor/Poller.h"
#include "AMS/Reactor/IHandler.h"
#include "AMS/Reactor/Dispatcher.h"
#include "AMS/Discovery/Aliveness.h"

namespace AMS {
    class IService {
    public:

        // Create or just return a singleton instance
        static IService& instance();
        // Destroy the singleton instance
        static void destroy();

        // Create a messaging domain restricted for communication
        void create_domain(std::string domainName, std::string selfDesc);

        // Create a subscriber for T-typed messages
        template<typename T>
        void create_subscriber();

        // Create a publisher for T-typed messages
        template<typename T>
        void create_publisher();

        // Subscribe a handler to T-typed messages dispatched automatically
        template<typename T>
        void subscribe(IHandler& handler);

        // Unsubscribe from receiving T-typed messages
        template<typename T>
        void unsubscribe();

        // Send a message to all subscribers
        void send_message(IMsgObj& obj);

        // Start/stop the reactor for the communication
        void reactor_start();
        void reactor_stop();

        // Register a notifier for peer status updates within the domain
        void register_discovery(IPeerNotification* notifier);

        // Return the host ip address
        std::string get_host_ip() const;

        // Return the service logger
        Poco::Logger& logger() { return *m_consoleLogger; }        

        // Run service in debug mode
        void debug_mode() { m_consoleLogger->setLevel("debug"); }

    protected:

        IService();
        ~IService();
        IService(const IService&);
        IService& operator=(const IService&);

        void initialize(Socket& socket, socket_type type, MessageId_t messageId, std::string hname);

        void run_activity();

        void create_logger();

        void handle_new_peer(const Heartbeat& hbeat);

        void create_single_publisher();

    private:

        static IService* s_instance;
        static Poco::FastMutex s_mutex;

        Context m_context;
        Poller m_poller;
        Dispatcher m_dispatcher;
        Discovery m_discovery;        

        typedef std::map<MessageId_t, IPubSub*> Pubsub_t;
        Pubsub_t m_publishers;
        Pubsub_t m_subscribers;

        Socket* m_subsocket;
        Socket* m_pubsocket;

        Poco::Activity<IService> m_activity;
        Poco::FastMutex m_mutex;

        Poco::Logger* m_consoleLogger;

        Aliveness m_aliveness;
        friend class Aliveness;        

        std::vector<std::string> m_staticPeers;

        PollId_t m_pollId;
        int m_domainId;

        std::string m_hostIP;

        bool m_initialised;        
    };    

    template<typename T>
    void IService::create_subscriber() {
        T obj;
        int messageId = obj.getMessageObjectId();
        Poco::FastMutex::ScopedLock lock(m_mutex);
        if (m_subscribers.find(messageId) == m_subscribers.end()) {            
            m_subscribers[messageId] = new PubSub<T>(*m_subsocket); //fixme: leak                   
        }
    }

    template<typename T>
    void IService::create_publisher() {
        T obj;
        int messageId = obj.getMessageObjectId();
        Poco::FastMutex::ScopedLock lock(m_mutex);
        if (m_publishers.find(messageId) == m_publishers.end()) {        
            if (!m_pubsocket) {
                create_single_publisher();
            }
            m_publishers[messageId] = new PubSub<T>(*m_pubsocket); //fixme: leak           
        }
    }

    template<typename T>
    void IService::subscribe(IHandler& handler) {
        T obj;
        int messageId = obj.getMessageObjectId();
        Poco::FastMutex::ScopedLock lock(m_mutex);
        if (m_subscribers.find(messageId) != m_subscribers.end()) {
            IPubSub* pubsub = m_subscribers[messageId];
            m_dispatcher.registerHandler(messageId, handler);
            pubsub->subscribe();
        }
    }

    template<typename T>
    void IService::unsubscribe() {
        T obj;
        int messageId = obj.getMessageObjectId();
        Poco::FastMutex::ScopedLock lock(m_mutex);
        if (m_subscribers.find(messageId) != m_subscribers.end()) {
            IPubSub* pubsub = m_subscribers[messageId];
            pubsub->unsubscribe();
            m_dispatcher.unregisterHandler(messageId);
        }
    }    
} //namespace AMS
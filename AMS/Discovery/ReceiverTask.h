#pragma once

#include "AMS/Types.h"
#include "AMS/Net/Serializer.h"
#include "AMS/Discovery/IPeerNotification.h"
#include "AMS/Reactor/IHandler.h"

namespace AMS {
    const Poco::Timespan::TimeDiff KWaitUSec(250000);	// 250msec
    const unsigned int EXPIRY_TIME = 5000000;
    
    class HeartbeatHandler;
    class ReceiverTask: public Poco::Task
    {
    public:
        // args[0]: MulticastAddress, args[1]: MulticastPort
        ReceiverTask();
        ~ReceiverTask();

        void runTask();

        void initialize(const std::vector<std::string>& args, const Poco::UUID& uuid);

        void handleBeacon(const Heartbeat& hbeat);

        void addNotifier(IPeerNotification* notifier);

        void selfNotify(const Heartbeat&);

        void destroy();

    private:

        Poco::Net::IPAddress::Family m_family;
        Poco::UInt16 m_port;
        Poco::Net::MulticastSocket m_multicastSocket;
        Poco::Net::SocketAddress m_socketAddres;
        Poco::Net::NetworkInterface m_if;

        Poco::UUID m_self;
        bool m_selfrcv;

        typedef std::map<Poco::UUID, Heartbeat> PeersMap;
        PeersMap m_peers;

        typedef std::vector<IPeerNotification*> PeerNotifications;
        PeerNotifications m_notifiers;
        Poco::Timestamp m_ts;

        char m_buffer[8192];

        HeartbeatHandler* m_hb_handler;

        Poco::FastMutex m_mutex;
    };

    class HeartbeatHandler : public AMS::IHandler {
    public:
        HeartbeatHandler(ReceiverTask* receiver) : m_receiver(receiver) {}

        virtual void handle(AMS::IMsgObj* baseMsg) {
            Heartbeat* msg = dynamic_cast<Heartbeat*>(baseMsg);
            if (!m_receiver->isCancelled() && msg != 0) {                   
                m_receiver->handleBeacon(*msg);
            }
        }

    private:
        ReceiverTask* m_receiver;
    };
}
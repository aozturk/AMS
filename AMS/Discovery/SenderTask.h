#pragma once

#include "AMS/Net/Serializer.h"
#include "AMS/Types.h"
#include "AMS/Discovery/Heartbeat.h"

namespace AMS {
    class SenderTask
    {
    public:
        
        SenderTask();
        ~SenderTask();

        // Initialize the discovery sender
        // args[0]: MulticastAddress, args[1]: MulticastPort
        const Poco::UUID& initialize(const std::vector<std::string>& args);

        void onTimer(Poco::Timer& timer);

        void addPub(unsigned port);

        void addDesc(std::string desc);

        const Heartbeat& getSelfInfo() const;

    protected:

        void generateUUID();

    private:

        Poco::Net::IPAddress::Family m_family;
        Poco::UInt16 m_port;
        Poco::Net::MulticastSocket m_multicastSocket;
        Poco::Net::SocketAddress* m_pMulticastSocketAddress;
        std::string m_hostAddress;
        Poco::UUID m_uuid;
        Heartbeat m_heartbeat;
        Poco::FastMutex m_mutex;
    };
}
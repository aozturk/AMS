#pragma once

#include "AMS/Discovery/ReceiverTask.h"
#include "AMS/Discovery/SenderTask.h"
#include "AMS/Config/Configurator.h"

namespace AMS {
    class Discovery {
    public:
        Discovery();

        void startDiscovery(int domainId);

        void stopDiscovery();

        void registerDiscovery(IPeerNotification* notifier);

        void addPub(unsigned port);

        void addDesc(std::string desc);

        void selfNotify();

    private:

        Poco::TaskManager m_tm;
        SenderTask m_senderTask;
        ReceiverTask* m_receiverTask;
        Poco::Timer m_timer;
    };
} //namespace AMS
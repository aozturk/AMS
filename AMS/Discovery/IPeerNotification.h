#pragma once 

#include "AMS/Discovery/Heartbeat.h"

namespace AMS {
    class IPeerNotification {
    public:
        IPeerNotification() {}
        virtual ~IPeerNotification() {}

        virtual void OnAlive(const Heartbeat& hbeat) = 0;
        virtual void OnUpdate(const Heartbeat& hbeat) = 0;
        virtual void OnDead(const Heartbeat& hbeat) = 0;
    };
}
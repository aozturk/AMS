#pragma once

#include "AMS/Discovery/Discovery.h"

namespace AMS {
    class Aliveness : public IPeerNotification {
    public:
        virtual void OnAlive(const Heartbeat& hbeat);
        virtual void OnUpdate(const Heartbeat& hbeat);
        virtual void OnDead(const Heartbeat& hbeat);

        void AddConnectedPub(std::string host, int pub) { 
            m_connectedPubs.insert(std::pair<std::string, int>(host, pub)); 
        }

        bool IsPubConnected(std::string host, int pub) { 
            return m_connectedPubs.find(std::pair<std::string, int>(host, pub)) 
                != m_connectedPubs.end(); 
        }

    private:
        std::set< std::pair<std::string, int> > m_connectedPubs; 
    };
}

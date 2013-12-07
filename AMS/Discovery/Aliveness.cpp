#include "Aliveness.h"
#include "AMS/IService.h"

using namespace AMS;

void Aliveness::OnAlive(const Heartbeat& hbeat) {
    poco_information_f3(IService::instance().logger(), 
        "OnAlive: peer on %s:%d (%s)", hbeat.host, hbeat.pubs.begin()->first, hbeat.uuid);
    IService::instance().handle_new_peer(hbeat);    
}

void Aliveness::OnUpdate(const Heartbeat& hbeat) {
    poco_information_f3(IService::instance().logger(), 
        "OnUpdate: peer on %s:%d (%s)", hbeat.host, hbeat.pubs.begin()->first, hbeat.uuid);
    IService::instance().handle_new_peer(hbeat);    
}

void Aliveness::OnDead(const Heartbeat& hbeat) {
    poco_information_f3(IService::instance().logger(), 
        "OnDead: peer on %s:%d (%s)", hbeat.host, hbeat.pubs.begin()->first, hbeat.uuid);
}
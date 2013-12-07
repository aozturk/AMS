#pragma once

#include "AMS/Discovery/Discovery.h"

using namespace AMS;

Discovery::Discovery() 
    : m_timer(0, 2500) {
        m_receiverTask = new ReceiverTask();
}

void Discovery::startDiscovery(int domainId)
{
    std::vector<std::string> args;
    args.push_back(Configurator::getMcastAddress());
    args.push_back(Configurator::getMcastPort_str(domainId));

    const Poco::UUID& uuid = m_senderTask.initialize(args);

    m_timer.start(Poco::TimerCallback<SenderTask>(m_senderTask, &SenderTask::onTimer));
        
    m_receiverTask->initialize(args, uuid);
    m_tm.start(m_receiverTask);
}

void Discovery::stopDiscovery()
{    
    m_timer.stop();
    m_receiverTask->destroy();
//    m_tm.joinAll();
}

void Discovery::registerDiscovery(IPeerNotification* notifier)
{
    m_receiverTask->addNotifier(notifier);
}

void Discovery::addPub(unsigned port) {
    m_senderTask.addPub(port);
}

void Discovery::addDesc(std::string desc) {
    m_senderTask.addDesc(desc);
}

void Discovery::selfNotify() {
    m_receiverTask->selfNotify(m_senderTask.getSelfInfo());
}

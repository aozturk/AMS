#include "AMS/Discovery/SenderTask.h"
#include "AMS/IService.h"

using namespace AMS;

SenderTask::SenderTask()
{    
}

SenderTask::~SenderTask()
{
    if(NULL != m_pMulticastSocketAddress)
    {
        delete m_pMulticastSocketAddress;
    }
}

const Poco::UUID& SenderTask::initialize(const std::vector<std::string>& args) {
    // Heartbeat object in serializer will be changed
    Poco::FastMutex::ScopedLock lock(m_mutex);

    m_family = Poco::Net::IPAddress(args[0]).family();
    m_port = Poco::NumberParser::parseUnsigned(args[1]);
    m_multicastSocket = Poco::Net::MulticastSocket(m_family);

    m_pMulticastSocketAddress =
        new Poco::Net::SocketAddress(Poco::Net::IPAddress(args[0]), m_port);

    // Finds host address from network interface
    m_hostAddress = findInterface(m_family).address().toString();
    
    // Generates UUID for this application
    generateUUID();    

    m_heartbeat.host = m_hostAddress;
    m_heartbeat.uuid = m_uuid.toString();

    IService::instance().create_publisher<Heartbeat>();

    IService::instance().logger().information(
        Poco::format("discovery sender running: %s UUID:%s [version=%d]", m_pMulticastSocketAddress->toString(),
        m_uuid.toString(), static_cast<int>(m_uuid.version())));

    return m_uuid;
}

void SenderTask::onTimer(Poco::Timer& timer)
{
    // Do not allow change in heartbeat object in serializer
    Poco::FastMutex::ScopedLock lock(m_mutex);

    if(NULL != m_pMulticastSocketAddress)
    {
        msgpack::sbuffer sbuf;
        msgpack::pack(sbuf, m_heartbeat);

        std::size_t n = m_multicastSocket.sendTo(	sbuf.data(),
            sbuf.size(), *m_pMulticastSocketAddress);
    }

    IService::instance().send_message(m_heartbeat);
}

void SenderTask::generateUUID() {
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    // Create a time-based or random (if MAC address unavailable) UUID
    m_uuid = generator.createOne();
}

void SenderTask::addPub(unsigned port) {
    // Heartbeat object in serializer will be changed
    Poco::FastMutex::ScopedLock lock(m_mutex);

    // Insert the new listening port
    m_heartbeat.pubs[port] = true;
}

void SenderTask::addDesc(std::string desc) {
    // Heartbeat object in serializer will be changed
    Poco::FastMutex::ScopedLock lock(m_mutex);

    // Insert the new listening port
    m_heartbeat.desc = desc;
}

const Heartbeat& SenderTask::getSelfInfo() const {
    return m_heartbeat;
}
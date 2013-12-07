#pragma once

#include "Socket.h"

using namespace AMS;

Socket::Socket(Context& context, socket_type type)
    : m_socket(NULL), m_type(type)
{
    m_socket = zmq_socket(context, static_cast<int>(type));
    ZMQ_ASSERT (m_socket != NULL);
}

Socket::~Socket() {
    if (NULL != m_socket)
    {
        int result = zmq_close(m_socket);
        ZMQ_ASSERT(0 == result);
        m_socket = NULL;
    }
}

socket_type Socket::socktype() const {
    return m_type;
}

int Socket::bind(const std::string& endpoint) {
    int result = zmq_bind(m_socket, endpoint.c_str());
    return result;
}

void Socket::connect(const std::string& endpoint) {
    int result = zmq_connect(m_socket, endpoint.c_str());
    ZMQ_ASSERT(0 == result);
}

std::string Socket::generateEndpoint(transport protocol, const std::string& inter_face,
    unsigned long port, const std::string& mcast_addr) {
        std::string endpoint_str;
        switch (protocol) {
        case TCP: {
            //format "tcp://host:port"
            std::ostringstream endpoint;
            endpoint << "tcp" << "://" << inter_face << ":" << port;
            endpoint_str = endpoint.str();
                  }
                  break;
        case EPGM: {
            //format "epgm://eth0;239.192.1.1:5555"
            std::ostringstream endpoint;
            endpoint << "epgm" << "://" << inter_face << ";" << mcast_addr << ":" << port;
            endpoint_str = endpoint.str();
                   }
                   break;
        case PGM: {
            //format "epgm://eth0;239.192.1.1:5555"
            std::ostringstream endpoint;
            endpoint << "pgm" << "://" << inter_face << ";" << mcast_addr << ":" << port;
            endpoint_str = endpoint.str();
                   }
                   break;
                  //TODO: other protocols
        default:
            assert(false); //for now
            break;
        }
        return endpoint_str;
}

int Socket::bind(transport protocol, const std::string& inter_face,
    unsigned long port, const std::string& mcast_addr) 
{
    int ret;
    do {
        ret = bind(generateEndpoint(protocol, inter_face, port, mcast_addr));
        port += 1;
    } while(ret != 0);
    return port - 1;
}

void Socket::connect(transport protocol, const std::string& inter_face,
    unsigned long port, const std::string& mcast_addr) {
        connect(generateEndpoint(protocol, inter_face, port, mcast_addr));
}

void Socket::close() {
    int result = zmq_close(m_socket);
    ZMQ_ASSERT(0 == result);
    m_socket = NULL;
}

bool Socket::send(SingleMessage& message, bool dont_block)
{
    int flag = NORMAL;
    if (dont_block) {
        // the operation is performed in non-blocking mode
        flag |= DONT_WAIT;
    }

    int result = zmq_msg_send(&message.raw_msg(), m_socket, flag);

    if (result < 0)
    {
        // non-blocking mode was requested and the message cannot be sent at the moment
        if(EAGAIN == zmq_errno())
        {
            return false;
        }

        // assert otherwise
        ZMQ_ASSERT(EAGAIN == zmq_errno());
    }

    message.sent();

    return true;
}

//TODO: send multi-part messages (see: ZMQ_SNDMORE)

bool Socket::send_raw(const char* buffer, int length, int flags)
{
    int result = zmq_send(m_socket, buffer, length, flags);

    if (result < 0)
    {
        // non-blocking mode was requested and the message cannot be sent at the moment
        if(EAGAIN == zmq_errno())
        {
            return false;
        }

        // assert otherwise
        ZMQ_ASSERT(EAGAIN == zmq_errno());
    }

    return true;
}

bool Socket::receive(SingleMessage& message, bool dont_block) {
    int flags = (dont_block) ? DONT_WAIT : NORMAL;

    int result = zmq_msg_recv(&message.raw_msg(), m_socket, flags);

    if (result < 0)
    {
        if (EAGAIN == zmq_errno())
        {
            return false;
        }

        ZMQ_ASSERT(EAGAIN == zmq_errno());
    }

    return true;
}

//TODO: receive multi-part messages (see: ZMQ_RCVMORE)

bool Socket::receive_raw(char* buffer, int& length, int flags)
{
    int result = zmq_recv(m_socket, &buffer, length, flags);
    ZMQ_ASSERT(result <= length); // message truncation

    if (result < 0)
    {
        if (EAGAIN == zmq_errno())
        {
            return false;
        }

        ZMQ_ASSERT(EAGAIN == zmq_errno());
    }

    if (result < length) {
        buffer[result] = 0;
    }

    length = result;

    return true;
}

// Helper
void Socket::subscribeTopic(const std::string& topic)
{
    set(SUBSCRIBE, topic);
}

void Socket::unsubscribeTopic(const std::string& topic)
{
    set(UNSUBSCRIBE, topic);
}

// Set socket options for different types of option
void Socket::set(socket_option const& option, int const& value)
{
    switch(option)
    {
        // unsigned 64bit Integers
    case AFFINITY:
        if (value < 0) { ZMQ_EXCEPTION("attempting to set an unsigned 64 bit integer option with a negative integer"); }
        set(option, static_cast<uint64_t>(value));
        break;

        // 64bit Integers
    case MAX_MESSAGE_SIZE:
        set(option, static_cast<int64_t>(value));
        break;

        // Boolean
    case IPV4_ONLY:
        if (value == 0) { set(option, false); }
        else if (value == 1) { set(option, true); }
        else { ZMQ_EXCEPTION("attempting to set a boolean option with a non 0 or 1 integer"); }
        break;

        // Integers that require +ve numbers
    case RECONNECT_INTERVAL_MAX:
    case SEND_BUFFER_SIZE:
    case RECOVERY_INTERVAL:
    case RECEIVER_BUFFER_SIZE:
    case SEND_HIGH_WATER_MARK:
    case RECEIVE_HIGH_WATER_MARK:
    case MULTICAST_HOPS:
    case RATE:
        if (value < 0) { ZMQ_EXCEPTION("attempting to set a positive only integer option with a negative integer"); }
        ZMQ_ASSERT (0 == zmq_setsockopt(m_socket, static_cast<int>(option), &value, sizeof(value)));
        break;
    case RECONNECT_INTERVAL:
    case LINGER:
    case BACKLOG:
    case RECEIVE_TIMEOUT:
    case SEND_TIMEOUT:
        ZMQ_ASSERT (0 == zmq_setsockopt(m_socket, static_cast<int>(option), &value, sizeof(value)));
        break;
    default:
        ZMQ_EXCEPTION("attempting to set a non signed integer option with a signed integer value");
    }
}

void Socket::set(socket_option const& option, bool const& value)
{
    switch(option)
    {
    case IPV4_ONLY:
        zmq_setsockopt(m_socket, static_cast<int>(option), &value, sizeof(value));
        break;
    default:
        ZMQ_EXCEPTION("attempting to set a non boolean option with a boolean value");
    }
}

void Socket::set(socket_option const& option, uint64_t const& value)
{
    switch(option)
    {
    case AFFINITY:
        ZMQ_ASSERT (0 == zmq_setsockopt(m_socket, static_cast<int>(option), &value, sizeof(value)));
        break;
    default:
        ZMQ_EXCEPTION("attempting to set a non unsigned 64 bit integer option with a unsigned 64 bit integer value");
    }
}

void Socket::set(socket_option const& option, int64_t const& value)
{
    switch(option)
    {
    case MAX_MESSAGE_SIZE:
        // zmq only allowed +ve int64_t options
        if (value < 0) { ZMQ_EXCEPTION("attempting to set a positive only 64 bit integer option with a negative 64bit integer"); }
        ZMQ_ASSERT (0 == zmq_setsockopt(m_socket, static_cast<int>(option), &value, sizeof(value)));
        break;
    default:
        ZMQ_EXCEPTION("attempting to set a non 64 bit integer option with a 64 bit integer value");
    }
}

void Socket::set(socket_option const& option, const std::string& value)
{
    switch(option)
    {
    case IDENTITY:
    case SUBSCRIBE:
    case UNSUBSCRIBE:
        ZMQ_ASSERT (0 == zmq_setsockopt(m_socket, static_cast<int>(option), static_cast<const void *>(value.c_str()), value.size()));
        break;
    default:
        ZMQ_EXCEPTION("attempting to set a non string option with a string value");
    }
}

// Get socket options, multiple versions for easy of use
void Socket::get(socket_option const& option, int& value) const
{
    size_t value_size = sizeof(int);

    switch(option)
    {
    case TYPE:
    case LINGER:
    case BACKLOG:
    case RECONNECT_INTERVAL:
    case RECONNECT_INTERVAL_MAX:
    case RECEIVE_TIMEOUT:
    case SEND_TIMEOUT:
    case FILE_DESCRIPTOR:
    case EVENTS:
    case RECEIVE_MORE:
    case SEND_BUFFER_SIZE:
    case RECEIVER_BUFFER_SIZE:
    case RATE:
    case RECOVERY_INTERVAL:
    case SEND_HIGH_WATER_MARK:
    case RECEIVE_HIGH_WATER_MARK:
    case MULTICAST_HOPS:
    case IPV4_ONLY:
#ifdef ZMQ_EXPERIMENTAL_LABELS
    case receive_label:
#endif
        ZMQ_ASSERT (0 == zmq_getsockopt(m_socket, static_cast<int>(option), &value, &value_size));

        // sanity check
        assert(value_size <= sizeof(int));
        break;
    default:
        ZMQ_EXCEPTION("attempting to get a non integer option with an integer value");
    }
}

void Socket::get(socket_option const& option, bool& value) const
{
    int int_value = 0;
    size_t value_size = sizeof(int);

    switch(option)
    {
    case RECEIVE_MORE:
    case IPV4_ONLY:
#ifdef ZMQ_EXPERIMENTAL_LABELS
    case receive_label:
#endif
        ZMQ_ASSERT (0 == zmq_getsockopt(m_socket, static_cast<int>(option), &int_value, &value_size));

        value = (int_value == 1) ? true : false;
        break;
    default:
        ZMQ_EXCEPTION("attempting to get a non boolean option with a boolean value");
    }
}

void Socket::get(socket_option const& option, uint64_t& value) const
{
    size_t value_size = sizeof(uint64_t);

    switch(option)
    {
    case AFFINITY:
        ZMQ_ASSERT(0 == zmq_getsockopt(m_socket, static_cast<int>(option), &value, &value_size));
        break;
    default:
        ZMQ_EXCEPTION("attempting to get a non unsigned 64 bit integer option with an unsigned 64 bit integer value");
    }
}

void Socket::get(socket_option const& option, int64_t& value) const
{
    size_t value_size = sizeof(int64_t);

    switch(option)
    {
    case MAX_MESSAGE_SIZE:
        ZMQ_ASSERT(0 == zmq_getsockopt(m_socket, static_cast<int>(option), &value, &value_size));
        break;
    default:
        ZMQ_EXCEPTION("attempting to get a non 64 bit integer option with an 64 bit integer value");
    }
}

void Socket::get(socket_option const& option, std::string& value) const
{
    size_t size = max_socket_option_buffer_size;
    char buffer[max_socket_option_buffer_size];

    switch(option)
    {
    case IDENTITY:
        ZMQ_ASSERT(0 == zmq_getsockopt(m_socket, static_cast<int>(option), buffer, &size));

        value.assign(buffer, size);
        break;
    default:
        ZMQ_EXCEPTION("attempting to get a non string option with a string value");
    }
}
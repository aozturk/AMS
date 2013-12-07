#pragma once

#include "AMS/Net/Context.h"
#include "AMS/Net/SingleMessage.h"
#include "AMS/Types.h"

namespace AMS {
    class Socket {
    public:

        Socket(Context& context, socket_type type);
        ~Socket();

        // Connection methods //
        socket_type socktype() const;

        int bind(transport protocol, const std::string& inter_face,
            unsigned long port, const std::string& mcast_addr="");

        void connect(transport protocol, const std::string& inter_face,
            unsigned long port, const std::string& mcast_addr="");

        void close();

        // Messaging methods //
        bool send(SingleMessage& message, bool dont_block = false);

        bool send_raw(const char* buffer, int length, int flags = NORMAL);

        bool receive(SingleMessage& message, bool dont_block = false);

        bool receive_raw(char* buffer, int& length, int flags = NORMAL);

        // Socket options specific methods //
        void subscribeTopic(const std::string& topic);

        void unsubscribeTopic(const std::string& topic);

        bool valid() const { return NULL != m_socket; }

        operator void*() const { return m_socket; }

    protected:
        int bind(const std::string& endpoint);
        void connect(const std::string& endpoint);

        std::string generateEndpoint(transport protocol, const std::string& inter_face,
            unsigned long port, const std::string& mcast_addr);

        // Set socket options for different types of option
        void set(socket_option const& option, int const& value);

        void set(socket_option const& option, bool const& value);

        void set(socket_option const& option, uint64_t const& value);

        void set(socket_option const& option, int64_t const& value);

        void set(socket_option const& option, const std::string& value);

        // Get socket options, multiple versions for easy of use
        void get(socket_option const& option, int& value) const;

        void get(socket_option const& option, bool& value) const;

        void get(socket_option const& option, uint64_t& value) const;

        void get(socket_option const& option, int64_t& value) const;

        void get(socket_option const& option, std::string& value) const;

        template<typename Type>
        Type get(socket_option option) const;

    private:
        void* m_socket;
        socket_type m_type;
        zmq_msg_t m_recv_buffer;

        // No copy
        Socket(const Socket&);
        Socket& operator=(const Socket&);
    };

    template<typename Type>
    Type Socket::get(socket_option option) const
    {
        Type value = Type();
        get(option, value);
        return value;
    }
}
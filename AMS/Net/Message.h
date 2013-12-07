#pragma once

#include "AMS/Helper.h"

namespace AMS {
    class Message {
    public:
        struct zmq_msg_wrapper
        {
            bool sent;
            zmq_msg_t msg;
        };

        typedef std::function<void (void*)> release_function;

        struct callback_releaser
        {
            release_function func;
        };

        Message() : m_parts()
            , m_read_cursor(0)
        {
        }
        ~Message()
        {
            for(size_t i = 0; i < m_parts.size(); ++i)
            {
                zmq_msg_t& msg = m_parts[i].msg;

                int result = zmq_msg_close(&msg);
                ZMQ_ASSERT(0 == result);
            }
            m_parts.clear();
        }

        size_t parts() const {
            return m_parts.size();
        }

        size_t size(size_t const& part) {
            if(part >= m_parts.size()) {
                return -1;
            }

            zmq_msg_wrapper& wrap = m_parts[part];
            return zmq_msg_size(&wrap.msg);
        }

        std::string get(size_t const& part) {
            return std::string(static_cast<char*>(raw_data(part)), size(part));
        }

        template<typename Type>
        void get(Type& value, size_t const& part)
        {
            size_t old = _read_cursor;
            _read_cursor = part;

            *this >> value;

            _read_cursor = old;
        }

        // Warn: If a pointer type is requested the message (well zmq) still 'owns'
        // the data and will release it when the message object is freed.
        template<typename Type>
        Type get(size_t const& part)
        {
            Type value;
            get(value, part);
            return value;
        }

        // Raw get data operations, useful with data structures more than anything else
        // Warn: The message (well zmq) still 'owns' the data and will release it
        // when the message object is freed.
        template<typename Type>
        void get(Type*& value, size_t const& part)
        {
            value = static_cast<Type*>(raw_data(part));
        }

        // Warn: The message (well zmq) still 'owns' the data and will release it
        // when the message object is freed.
        template<typename Type>
        void get(Type** value, size_t const& part)
        {
            *value = static_cast<Type*>(raw_data(part));
        }

        // Move operators will take ownership of message parts without copying
        void move(void* part, size_t& size, release_function const& release) {
            parts_type tmp(m_parts.size() + 1);

            for(size_t i = 0; i < m_parts.size(); ++i)
            {
                zmq_msg_t& dest = tmp[i].msg;
                ZMQ_ASSERT( 0 == zmq_msg_init(&dest) );

                zmq_msg_t& src = m_parts[i].msg;
                ZMQ_ASSERT( 0 == zmq_msg_move(&dest, &src) );
            }

            std::swap(tmp, m_parts);

            callback_releaser* hint = new callback_releaser();
            hint->func = release;

            zmq_msg_t& msg = m_parts.back().msg;
            ZMQ_ASSERT (0 == zmq_msg_init_data(&msg, part, size, &release_callback, hint));
        }

        // Raw move data operation, useful with data structures more than anything else
        template<typename Object>
        void move(Object *part)
        {
            move(part, sizeof(Object), &deleter_callback<Object>);
        }

        // Copy operators will take copies of any data
        void add(void const* part, size_t const& size) {
            parts_type tmp(m_parts.size() + 1);

            for(size_t i = 0; i < m_parts.size(); ++i)
            {
                zmq_msg_t& dest = tmp[i].msg;
                ZMQ_ASSERT( 0 == zmq_msg_init(&dest) );

                zmq_msg_t& src = m_parts[i].msg;
                ZMQ_ASSERT( 0 == zmq_msg_move(&dest, &src) );
            }

            std::swap(tmp, m_parts);

            zmq_msg_t& msg = m_parts.back().msg;

            ZMQ_ASSERT( 0 == zmq_msg_init_size(&msg, size) );

            void* msg_data = zmq_msg_data(&msg);

            memcpy(msg_data, part, size);
        }

        template<typename Type>
        void add(Type const& part)
        {
            *this << part;
        }

        // Move supporting
        Message(Message&& source) {
            std::swap(m_parts, source.m_parts);
        }
        Message& operator=(Message&& source) {
            std::swap(m_parts, source.m_parts);
            return *this;
        }

        // Copy support
        Message copy() {
            Message msg;
            msg.copy(*this);
            return msg;
        }

        void copy(Message& source) {
            m_parts.resize(source.m_parts.size());
            for(size_t i = 0; i < source.m_parts.size(); ++i)
            {
                ZMQ_ASSERT( 0 == zmq_msg_init_size(&m_parts[i].msg, zmq_msg_size(&source.m_parts[i].msg)) );

                ZMQ_ASSERT( 0 == zmq_msg_copy(&m_parts[i].msg, &source.m_parts[i].msg) );
            }

            // we don't need a copy of the releasers as we did data copies of the internal data,
            //_releasers = source._releasers;
            //_strings = source._strings
        }

        // Used for internal tracking
        void sent(size_t const& part) {
            // sanity check
            assert(!m_parts[part].sent);
            m_parts[part].sent = true;
        }

        // Access to raw zmq details
        void* raw_data(size_t const& part = 0)
        {
            ZMQ_ASSERT(part < m_parts.size());

            zmq_msg_wrapper& wrap = m_parts[part];
            return zmq_msg_data(&wrap.msg);
        }

        zmq_msg_t& raw_msg(size_t const& part = 0)
        {
            ZMQ_ASSERT(part < m_parts.size());

            zmq_msg_wrapper& wrap = m_parts[part];
            return wrap.msg;
        }

        zmq_msg_t& raw_new_msg()
        {
            parts_type tmp(m_parts.size() + 1);

            for(size_t i = 0; i < m_parts.size(); ++i)
            {
                zmq_msg_t& dest = tmp[i].msg;
                ZMQ_ASSERT( 0 == zmq_msg_init(&dest) );

                zmq_msg_t& src = m_parts[i].msg;
                ZMQ_ASSERT( 0 == zmq_msg_move(&dest, &src) );
            }

            std::swap(tmp, m_parts);

            zmq_msg_t& msg = m_parts.back().msg;
            ZMQ_ASSERT( 0 == zmq_msg_init(&msg) );

            return msg;
        }

    private:
        typedef std::vector<zmq_msg_wrapper> parts_type;
        parts_type m_parts;
        size_t m_read_cursor;

        // Disable implicit copy support, code must request a copy to clone
        Message(const Message&);
        Message& operator=(const Message&);

        static void release_callback(void* data, void* hint) {
            callback_releaser* releaser = static_cast<callback_releaser*>(hint);
            releaser->func(data);

            delete releaser;
        }

        template<typename Object>
        static void deleter_callback(void* data)
        {
            delete static_cast<Object*>(data);
        }
    };
}
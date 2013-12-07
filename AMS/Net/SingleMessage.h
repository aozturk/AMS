#pragma once

#include "AMS/Helper.h"

namespace AMS {
    // This class define single ØMQ message, which is a discrete unit of data passed
    // between applications or components of the same application.
    // ØMQ messages are considered to be opaque binary data.
    class SingleMessage {
    public:
        // Constructor
        SingleMessage(size_t size=0)
            : m_sent(false), m_init(false)
        {
            init(size);
        }

        SingleMessage(char* strMsg)
            : m_sent(false), m_init(false)
        {
            int size = strlen (strMsg) + 1;
            init(size);

            ((char*)raw_data())[size-1] = 0;
            memcpy (raw_data(), strMsg, size);
        }

        SingleMessage(void* rawMsg, int size)
            : m_sent(false), m_init(false)
        {
            init(size);

            memcpy (raw_data(), rawMsg, size);
        }

        // Destructor
        ~SingleMessage()
        {
            close();
        }

        // Initialise 0MQ message to represent an empty message
        void init(size_t size=0) {
            if (m_init == false) {
                if (size > 0) {
                    // Initialise ØMQ message of a specified size
                    int rc = zmq_msg_init_size(&m_msg, size);
                    ZMQ_ASSERT(rc == 0);
                }
                else {
                    int rc = zmq_msg_init(&m_msg);
                    ZMQ_ASSERT(rc == 0);
                }
                m_init = true;
            }
        }

        //TODO: initialising a message from a supplied buffer (use: zmq_msg_init_data)

        // Release 0MQ message
        void close() {
            int rc = zmq_msg_close(&m_msg);
            ZMQ_ASSERT(0 == rc);
        }

        // Retrieve message content size in bytes
        size_t size() {
            return zmq_msg_size(&m_msg);
        }

        // Used for internal tracking
        void sent() {
            // sanity check
            assert(!m_sent);
            m_sent = true;
        }

        // Retrieve a pointer to raw message content
        void* raw_data()
        {
            return zmq_msg_data(&m_msg);
        }

        // Retrieve a reference to raw 0MQ message
        zmq_msg_t& raw_msg()
        {
            return m_msg;
        }

    private:
        zmq_msg_t m_msg;
        bool m_sent;
        bool m_init;

        // Disable implicit copy support, code must request a copy to clone
        SingleMessage(const SingleMessage&);
        SingleMessage& operator=(const SingleMessage&);
    };
}
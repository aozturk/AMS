#pragma once

#include "AMS/Helper.h"

namespace AMS {
    // Thread-safe class representing internal zmq context
    class Context
    {
    public:

        // Initialise the 0mq context.
        Context ();
        ~Context ();

        // Closes the 0mq context.
        void close();

        // Sets the maximum number of sockets allowed on the context (default: 1024).
        void setMaxSockets(unsigned n);

        // Returns the maximum number of sockets allowed for this context.
        int getMaxSockets();

        // Sets number of I/O threads (default: 1)
        // This option only applies before creating any sockets on the context.
        void setIOThreads(unsigned n);

        // Returns the size of the ØMQ thread pool for this context.
        int getIOThreads();

        // Validity checking of the context
        operator bool() const { return NULL != m_context; }

        // Access to the raw 0mq context
        operator void*() { return m_context; }

    private:

        void *m_context;

        Context (const Context&);
        void operator = (const Context&);
    };

    inline Context::Context ()
    {
        m_context = zmq_ctx_new ();
        ZMQ_ASSERT (m_context != NULL);
    }

    inline Context::~Context ()
    {
        this->close();
    }

    inline void Context::close()
    {
        if (m_context == NULL)
            return;
        int rc = zmq_ctx_destroy (m_context);
        ZMQ_ASSERT(rc == 0);
        m_context = NULL;
    }

    inline void Context::setMaxSockets(unsigned n) {
        int rc = zmq_ctx_set (m_context, ZMQ_MAX_SOCKETS, n);
        ZMQ_ASSERT(rc == 0);
    }

    inline int Context::getMaxSockets() {
        int n = zmq_ctx_get (m_context, ZMQ_MAX_SOCKETS);
        ZMQ_ASSERT (n != -1);
        return n;
    }

    inline void Context::setIOThreads(unsigned n) {
        int rc = zmq_ctx_set (m_context, ZMQ_IO_THREADS, n);
        ZMQ_ASSERT(rc == 0);
    }

    inline int Context::getIOThreads() {
        int n = zmq_ctx_get (m_context, ZMQ_IO_THREADS);
        ZMQ_ASSERT (n != -1);
        return n;
    }

    //TODO: signal handling
}

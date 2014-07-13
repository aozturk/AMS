#pragma once

#include "AMS/Helper.h"

namespace AMS
{
    // Socket types allowed by zmq
    typedef enum socket_type {
        PAIR       = ZMQ_PAIR,
        PUB        = ZMQ_PUB,
        SUB        = ZMQ_SUB,
        PULL       = ZMQ_PULL,
        PUSH       = ZMQ_PUSH,
        REQ        = ZMQ_REQ,
        REP        = ZMQ_REP,
        XPUB       = ZMQ_XPUB,
        XSUB       = ZMQ_XSUB,
        XREQ       = ZMQ_XREQ,
        XREP       = ZMQ_XREP,
        // For completion
        ROUTER     = ZMQ_ROUTER,
        DEALER     = ZMQ_DEALER
    } socket_type;

    typedef enum socket_option {
        AFFINITY                  = ZMQ_AFFINITY,          /* I/O thread affinity */
        IDENTITY                  = ZMQ_IDENTITY,          /* Socket identity */
        SUBSCRIBE                 = ZMQ_SUBSCRIBE,         /* Add topic subscription - set only */
        UNSUBSCRIBE               = ZMQ_UNSUBSCRIBE,       /* Remove topic subscription - set only */
        RATE                      = ZMQ_RATE,              /* Multicast data rate */
        SEND_BUFFER_SIZE          = ZMQ_SNDBUF,            /* Kernel transmission buffer size */
        RECEIVER_BUFFER_SIZE      = ZMQ_RCVBUF,            /* Kernel receive buffer size */
        RECEIVE_MORE              = ZMQ_RCVMORE,           /* Can receive more parts - get only */
        FILE_DESCRIPTOR           = ZMQ_FD,                /* Socket file descriptor - get only */
        EVENTS                    = ZMQ_EVENTS,            /* Socket event flags - get only */
        TYPE                      = ZMQ_TYPE,              /* Socket type - get only */
        LINGER                    = ZMQ_LINGER,            /* Socket linger timeout */
        BACKLOG                   = ZMQ_BACKLOG,           /* Maximum length of outstanding connections - get only */

        RECOVERY_INTERVAL         = ZMQ_RECOVERY_IVL,      /* Multicast recovery interval in milliseconds */

        RECONNECT_INTERVAL        = ZMQ_RECONNECT_IVL,     /* Reconnection interval */
        RECONNECT_INTERVAL_MAX    = ZMQ_RECONNECT_IVL_MAX, /* Maximum reconnection interval */
        RECEIVE_TIMEOUT           = ZMQ_RCVTIMEO,          /* Maximum inbound block timeout */
        SEND_TIMEOUT              = ZMQ_SNDTIMEO,          /* Maximum outbound block timeout */

        MAX_MESSAGE_SIZE          = ZMQ_MAXMSGSIZE,        /* Maximum inbound message size */
        SEND_HIGH_WATER_MARK      = ZMQ_SNDHWM,            /* High-water mark for outbound messages */
        RECEIVE_HIGH_WATER_MARK   = ZMQ_RCVHWM,            /* High-water mark for inbound messages */
        MULTICAST_HOPS            = ZMQ_MULTICAST_HOPS,    /* Maximum number of multicast hops */
        IPV4_ONLY                 = ZMQ_IPV4ONLY,
#ifdef ZMQ_EXPERIMENTAL_LABELS
        RECEIVE_LABEL             = ZMQ_RCVLABEL,          /* Received label part - get only */
#endif
    } socket_option;

    typedef enum flag_type {
        NORMAL     = 0,
        DONT_WAIT  = ZMQ_DONTWAIT,
        SEND_MORE  = ZMQ_SNDMORE
    } flag_type;

    static const int max_socket_option_buffer_size = 256;

    static const std::string ANY = "*";

    static const int INVALID_ID = -1;

    typedef enum transport {
        INPROC  = 1,
        IPC     = 2,
        TCP     = 3,
        PGM     = 4,
        EPGM    = 5
    } transport;

    // Enumeration for poll events
    typedef enum {
        POLL_FOREVER   = -1,
        POLL_IMMEDIATE = 0,
        POLL_1_SEC = 1000
    } PollTimeout;

    typedef enum {
        AMS_POLL_IN        = ZMQ_POLLIN,
        AMS_POLL_OUT       = ZMQ_POLLOUT,
        AMS_POLL_ERROR     = ZMQ_POLLERR
    } PollEventType;

    typedef int PollId_t;
    typedef int MessageId_t;
} //namespace SockTypes

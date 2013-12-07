#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include <iostream>
#include <functional>
#include <ostream>
#include <fstream>
#include <sstream>

#include <zmq.h>
#include <msgpack.hpp>

#include <Poco/Mutex.h>
#include <Poco/Activity.h>
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/Message.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>
#include <Poco/Runnable.h>
#include <Poco/AutoPtr.h>
#include <Poco/TaskManager.h>
#include <Poco/NumberParser.h>
#include <Poco/Timestamp.h>
#include <Poco/UUID.h>
#include <Poco/UUIDGenerator.h>
#include <Poco/Timer.h>
#include "Poco/ThreadPool.h"
#include <Poco/Net/DNS.h>
#include <Poco/Net/NetworkInterface.h>
#include <Poco/Net/MulticastSocket.h>

static Poco::Net::NetworkInterface findInterface(Poco::Net::IPAddress::Family family)
{
    Poco::Net::NetworkInterface::NetworkInterfaceList ifs = Poco::Net::NetworkInterface::list();
    for(Poco::Net::NetworkInterface::NetworkInterfaceList::const_iterator itr=ifs.begin();
        itr!=ifs.end(); ++itr)
    {
        if(	!itr->address().isLoopback()
            && (family == itr->address().family())
#if !defined(POCO_OS_FAMILY_WINDOWS)
            && ('e' == itr->name()[0])
#endif
            && itr->address().isUnicast()
            )
        {
            return *itr;
        }
    }
    return Poco::Net::NetworkInterface();
}

#define ZMQ_ASSERT(_expression) \
{ \
    if (!(_expression)) { \
        fprintf (stderr, "Internal error %d: %s at %s, line %d.", zmq_errno(), zmq_strerror(zmq_errno()), __FILE__, __LINE__); \
        exit(1); }\
}

#define ZMQ_EXCEPTION(_message) \
{ \
    fprintf (stderr, "Internal exception at %s, line %d.", __FILE__, __LINE__); \
    exit(1); \
}

//  Provide random number from 0..(num-1)
#if (defined (__WINDOWS__))
#   define randof(num)  (int) ((float) (num) * rand () / (RAND_MAX + 1.0))
#else
#   define randof(num)  (int) ((float) (num) * random () / (RAND_MAX + 1.0))
#endif

//  Set simple random printable identity on socket
static void
    s_set_id (void *socket)
{
    char identity [10];
    sprintf (identity, "%04X-%04X", randof (0x10000), randof (0x10000));
    zmq_setsockopt (socket, ZMQ_IDENTITY, identity, strlen (identity));
}

//  Sleep for a number of milliseconds
static void
    s_sleep (int msecs)
{
#if (defined (__WINDOWS__))
    Sleep (msecs);
#else
    struct timespec t;
    t.tv_sec  =  msecs / 1000;
    t.tv_nsec = (msecs % 1000) * 1000000;
    nanosleep (&t, NULL);
#endif
}

//  Return current system clock as milliseconds
static int64_t
    s_clock (void)
{
#if (defined (__WINDOWS__))
    SYSTEMTIME st;
    GetSystemTime (&st);
    return (int64_t) st.wSecond * 1000 + st.wMilliseconds;
#else
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return (int64_t) (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
}

//  Print formatted string to stdout, prefixed by date/time and
//  terminated with a newline.
static void
    s_console (const char *format, ...)
{
    time_t curtime = time (NULL);
    struct tm *loctime = localtime (&curtime);
    char *formatted = (char *) malloc (20);
    if (!formatted) return;
    strftime (formatted, 20, "%y-%m-%d %H:%M:%S ", loctime);
    printf ("%s", formatted);
    free (formatted);

    va_list argptr;
    va_start (argptr, format);
    vprintf (format, argptr);
    va_end (argptr);
    printf ("\n");
}

//  Report 0MQ version number
//
static void
    s_version (void)
{
    int major, minor, patch;
    zmq_version (&major, &minor, &patch);
    std::cout << "Current 0MQ version is " << major << "." << minor << "." << patch << std::endl;
}

#define HB_MSG_ID 9998
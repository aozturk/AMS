#pragma once

#include "AMS/Net/Socket.h"
#include "AMS/Types.h"

namespace AMS {
    // This class multiplexes input/output events in
    // a level-triggered fashion over a set of sockets.
    class Poller {
    public:
        Poller()
            : m_itemCount(0) {
        }

        // Multiplex input/output events in a level-triggered fashion.
        int poll(long timeout);

        // Add a socket to the polling model and set which events to monitor.
        // Updates events of an existing socket.
        PollId_t add(Socket& socket, short events);

        // Add a socket descriptor to the polling model and set which events to monitor.
        PollId_t add(int fd, short events);

        bool hasInput(PollId_t pollId);

        // Get the event flags triggered for a poll item.
        short reventsById(PollId_t pollId) const;

        // Get the event flags triggered for a socket.
        short revents(Socket& socket) const;

        // Get the event flags triggered for a file descriptor.
        short revents(int descriptor) const;

    protected:

        // Appends new poll item to zmq_pollitem_t structure.
        int addPollItem(const zmq_pollitem_t& item);

    private:

        zmq_pollitem_t m_pollItems[1000];

        typedef std::map<void *, int> SocketPollItems;
        SocketPollItems m_sockets;

        typedef std::map<int, int> FdPollItems;
        FdPollItems m_fds;

        unsigned m_itemCount;
    };

    inline int Poller::poll(long timeout)
    {
        int rc = zmq_poll(m_pollItems, m_itemCount, timeout);
        ZMQ_ASSERT (rc != -1);
        return rc;
    }

    inline int Poller::addPollItem(const zmq_pollitem_t& item) {
        unsigned index = m_itemCount++;
        m_pollItems[index] = item;
        return index;
    }

    inline PollId_t Poller::add(Socket& socket, short events) {
        zmq_pollitem_t item = { (void*) socket, 0, events, 0 };

        PollId_t pollId = addPollItem(item);
        m_sockets[socket] = pollId;
        return pollId;
    }

    inline PollId_t Poller::add(int fd, short events) {
        zmq_pollitem_t item = { NULL, fd, events, 0 };

        PollId_t pollId = addPollItem(item);
        m_fds[fd] = pollId;
        return pollId;
    }

    inline short Poller::reventsById(PollId_t pollId) const
    {
        return m_pollItems[pollId].revents;
    }

    inline bool Poller::hasInput(PollId_t pollId) {
        return reventsById(pollId) & POLL_IN;
    }

    inline short Poller::revents(Socket& socket) const
    {
        SocketPollItems::const_iterator iter = m_sockets.find((void*) socket);
        if (m_sockets.end() == iter)
        {
            return 0;
        }
        else {
            return m_pollItems[(*iter).second].revents;
        }
    }

    inline short Poller::revents(int descriptor) const
    {
        FdPollItems::const_iterator iter = m_fds.find(descriptor);
        if (m_fds.end() == iter)
        {
            return 0;
        }
        else {
            return m_pollItems[(*iter).second].revents;
        }
    }
}
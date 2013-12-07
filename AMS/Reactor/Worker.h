#pragma once

#include "AMS/MsgObject.h"
#include "AMS/Reactor/IHandler.h"

namespace AMS {
    class WorkNotification: public Poco::Notification
        // The notification sent to worker threads.
    {
    public:
        typedef Poco::AutoPtr<WorkNotification> Ptr;

        WorkNotification(IMsgObj* data, IHandler* handler):
        _data(data), _handler(handler) {
        }

        IMsgObj* data() const { return _data; }

        IHandler* handler() const {  return _handler; }

        void destroy_data() {
            if (_data) {
                delete _data;
                _data = NULL;
            }
        }

    private:
        IMsgObj* _data;
        IHandler* _handler;
    };

    class Worker: public Poco::Runnable
        // A worker thread that gets work items
        // from a NotificationQueue.
    {
    public:
        Worker(const std::string& name, Poco::NotificationQueue& queue);

        void run();

        void stop();

    private:
        std::string        m_name;
        bool m_running;
        Poco::NotificationQueue& m_queue;
        static Poco::FastMutex   m_mutex;
    };
}
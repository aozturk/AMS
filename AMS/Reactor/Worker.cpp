#include "Worker.h"

using namespace AMS;

Poco::FastMutex Worker::m_mutex;

Worker::Worker(const std::string& name, Poco::NotificationQueue& queue):
m_name(name),
    m_queue(queue),
    m_running(true)
{
}

void Worker::stop() {
    m_running = false;
}

void Worker::run()
{
    while (m_running)
    {
        Poco::Notification::Ptr pNf(m_queue.waitDequeueNotification());
        if (pNf)
        {
            WorkNotification::Ptr pWorkNf = pNf.cast<WorkNotification>();
            if (m_running && pWorkNf)
            {
                {
                    Poco::FastMutex::ScopedLock lock(m_mutex);
                    pWorkNf->handler()->handle(pWorkNf->data());
                    pWorkNf->destroy_data(); 
                }
            }
        }
        // thread has been woken up; exit
        else break;
    }
}
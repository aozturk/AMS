#pragma once

#include "AMS/Helper.h"
#include "AMS/Types.h"
#include "AMS/Reactor/Worker.h"
#include "AMS/Reactor/IHandler.h"

namespace AMS {
    class Dispatcher {
    public:
        Dispatcher(int numWorkers = 1) {
            for (int i=0; i < numWorkers; ++i) {
                std::ostringstream ostr;
                ostr << i;
                Worker* worker = new Worker(ostr.str(), m_queue);
                m_workers.push_back(worker);
                // start worker threads
                Poco::ThreadPool::defaultPool().start(*worker);
            }
        }
        ~Dispatcher() {                     
            // stop all worker threads
            std::vector<Worker*>::iterator iter = m_workers.begin();
            while (iter != m_workers.end()) {
                (*iter)->stop();
                ++iter;
            }

            m_queue.wakeUpAll();
            Poco::ThreadPool::defaultPool().joinAll();

            // destroy workers
            for (iter = m_workers.begin(); iter != m_workers.end(); ++iter)
                delete *iter;
        }

        void dispatch(IMsgObj* msg_obj) {
            Poco::FastMutex::ScopedLock lock(m_mutex);
            HandlerMap::iterator iter = m_handlers.find(msg_obj->getMessageObjectId());
            if (iter != m_handlers.end()) {
                m_queue.enqueueNotification(new WorkNotification(msg_obj, (*iter).second));
            }
        }

        void registerHandler(MessageId_t id, IHandler& handler) {
            Poco::FastMutex::ScopedLock lock(m_mutex);
            m_handlers[id] = &handler;
        }

        void unregisterHandler(MessageId_t id) {
            Poco::FastMutex::ScopedLock lock(m_mutex);
            m_handlers.erase(id);
        }

    private:

        Poco::NotificationQueue m_queue;
        std::vector<Worker*> m_workers;

        typedef std::map<MessageId_t, IHandler*> HandlerMap;
        HandlerMap m_handlers;

        Poco::FastMutex m_mutex;
    };
}
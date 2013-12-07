#pragma once

#include "AMS/Helper.h"

namespace AMS {
    class IMsgObj {
    public:

        IMsgObj(int msgObjectId) : m_msgObjectId(msgObjectId) {}
        virtual ~IMsgObj() {}

        int getMessageObjectId() {
            return m_msgObjectId;
        }

        void setMessageObjectId(int msgObjectId) {
            m_msgObjectId = msgObjectId;
        }

    private:

        int m_msgObjectId;

    };
}
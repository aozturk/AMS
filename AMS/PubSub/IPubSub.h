#pragma once

#include "AMS/MsgObject.h"
#include "AMS/Net/Socket.h"

namespace AMS {
    class IPubSub {
    public:
        IPubSub() {}
        virtual ~IPubSub() {}

        virtual Socket& getSocket() = 0;

        virtual void subscribe() = 0;
        virtual void unsubscribe() = 0;

        virtual IMsgObj* receive_message(SingleMessage& message) = 0;

        virtual void send_message(IMsgObj& testMsg) = 0;

        static const int MSG_NAME_SIZE = 16;

    };
}
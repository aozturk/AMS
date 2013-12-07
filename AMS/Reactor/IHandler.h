#pragma once

#include "AMS/MsgObject.h"

namespace AMS {
    class IHandler {
    public:
        virtual void handle(IMsgObj* baseMsg) = 0;
    };
}
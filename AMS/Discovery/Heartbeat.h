#pragma once 

#include <stdio.h>
#include "AMS/Helper.h"
#include "AMS/MsgObject.h"

namespace AMS {
    class Heartbeat : public IMsgObj {
    public:
        Heartbeat() : IMsgObj(HB_MSG_ID) {} 

        std::string uuid;
        std::string host;
        std::map<int, bool> pubs;
        std::string desc;

        unsigned long expires_at;

        Heartbeat& operator=(const Heartbeat& rhs) {
            if (this != &rhs) {
                host = rhs.host;
                uuid = rhs.uuid;
                pubs = rhs.pubs;
                desc = rhs.desc;
            }
            return *this;
        }

    public:
        MSGPACK_DEFINE(host, uuid, pubs, desc);
    };
}
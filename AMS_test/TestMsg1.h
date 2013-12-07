#pragma once

#include "AMS/MsgObject.h"

class ExtInfo {
public:
    std::string address;
    double offset;

    MSGPACK_DEFINE(address, offset);
};

class TestMsg1 : public AMS::IMsgObj {
public:
    TestMsg1() : IMsgObj(/*msg id*/11) {
    }

    std::string name;
    double value;
    ExtInfo info;
    std::vector<int> vec;
    std::map<int, int> map;

    // macro to serialize selected members only
    MSGPACK_DEFINE(name, value, info);
};

#pragma once

#include "AMS/MsgObject.h"

class TestMsg2 : public AMS::IMsgObj {
public:
    TestMsg2() : IMsgObj(/*msg id*/12) {
    }

    std::string name;
    double value;
    std::vector<int> vec;
    std::map<int, int> map;

    // macro to serialize selected members only
    MSGPACK_DEFINE(name, value, vec, map);
};

class TestMsg3 : public AMS::IMsgObj {
public:
    TestMsg3() : IMsgObj(/*msg id*/13) {
    }

    std::string name;
    double value;
    std::vector<int> vec;
    std::map<int, int> map;

    // macro to serialize selected members only
    MSGPACK_DEFINE(name, value, vec, map);
};

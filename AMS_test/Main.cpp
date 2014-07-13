#include "TestMsg1.h"
#include "TestMsg2.h"
#include "AMS/IService.h"
#include "AMS/Config/Parser.h"

using namespace std;

class Msg1Handler : public AMS::IHandler {
public:
    virtual void handle(AMS::IMsgObj* baseMsg) {
        TestMsg1* msg1 = dynamic_cast<TestMsg1*>(baseMsg);
        if (msg1 != 0) {
            poco_information_f3(AMS::IService::instance().logger(),
                "msg %d received! value: %.1f %s",
                msg1->getMessageObjectId(), msg1->value, msg1->name);
            return;
        }
    }
};

class Msg2Handler : public AMS::IHandler {
public:
    virtual void handle(AMS::IMsgObj* baseMsg) {
        TestMsg2* msg2 = dynamic_cast<TestMsg2*>(baseMsg);
        if (msg2 != 0) {
            poco_information_f3(AMS::IService::instance().logger(),
                "msg %d received! value: %.1f %s",
                msg2->getMessageObjectId(), msg2->value, msg2->name);

            TestMsg3 msg_3;
            msg_3.value = 56;
            msg_3.name = "ouchhh..!";
            AMS::IService::instance().send_message(msg_3);

            return;
        }
    }
};

class Msg3Handler : public AMS::IHandler {
public:
    virtual void handle(AMS::IMsgObj* baseMsg) {
        TestMsg3* msg3 = dynamic_cast<TestMsg3*>(baseMsg);
        if (msg3 != 0) {
            poco_information_f3(AMS::IService::instance().logger(),
                "msg %d received! value: %.1f %s",
                msg3->getMessageObjectId(), msg3->value, msg3->name);
            return;
        }
    }
};

class PeerHandler : public AMS::IPeerNotification {
public:
    void OnAlive(const AMS::Heartbeat& hbeat) {
        AMS::IService::instance().logger().information(
            Poco::format("OnAlive: peer on %s (%s)", hbeat.host, hbeat.uuid));
    }
    void OnUpdate(const AMS::Heartbeat& hbeat) {
        AMS::IService::instance().logger().information(
            Poco::format("OnUpdate: peer on %s (%s)", hbeat.host, hbeat.uuid));
    }
    void OnDead(const AMS::Heartbeat& hbeat) {
        AMS::IService::instance().logger().information(
            Poco::format("OnDead: peer on %s (%s)", hbeat.host, hbeat.uuid));
    }
};

void sub() {
    AMS::IService& service = AMS::IService::instance();

    service.logger().information("subscriber side running...");
    service.debug_mode();
    service.create_domain("ams_test", "Test_SUB");

    service.create_publisher<TestMsg1>();
    Msg1Handler handler1;
    service.create_subscriber<TestMsg1>();
    service.subscribe<TestMsg1>(handler1);

    Msg2Handler handler2;
    service.create_subscriber<TestMsg2>();
    service.subscribe<TestMsg2>(handler2);

    service.create_publisher<TestMsg3>();
    Msg3Handler handler3;
    service.create_subscriber<TestMsg3>();
    service.subscribe<TestMsg3>(handler3);

    service.reactor_start();

    for (int i=0; i<100; ++i)
    {
        TestMsg1 msg_1;
        msg_1.value = 89;
        msg_1.name = "seriuz..!";
        AMS::IService::instance().send_message(msg_1);

        Poco::Thread::sleep(100);
    }
}

void pub() {
    AMS::IService& service = AMS::IService::instance();

    service.logger().information("publisher side running...");
    service.debug_mode();
    service.create_domain("ams_test", "Test_PUB");

    service.create_publisher<TestMsg1>();

    service.create_publisher<TestMsg2>();

    Msg3Handler handler3;
    service.create_subscriber<TestMsg3>();
    service.subscribe<TestMsg3>(handler3);

    service.reactor_start();

    PeerHandler peer_handler;
    service.register_discovery(&peer_handler);

    for (int i=0; i<100; ++i)
     {
        TestMsg1 msg_1;
        msg_1.value = 23;
        msg_1.name = "testing..!";
        service.send_message(msg_1);

        TestMsg2 msg_2;
        msg_2.value = 45;
        msg_2.name = "damn..!";
        service.send_message(msg_2);

        Poco::Thread::sleep(100);
    }
}

int main(int argc, char* argv[]) {

    if (argc == 2) {
        if (!strcmp(argv[1], "--pub")) {
            pub();
        }
        else if (!strcmp(argv[1], "--sub")) {
            sub();
        }
    }

    AMS::IService::destroy();

    return 0;
}

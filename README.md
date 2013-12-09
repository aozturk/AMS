Asynchronous Messaging Service (AMS)
===

Features
--------

#### Asynchronous
Asynchronous message passing systems deliver a message from sender to receiver, without waiting for the receiver to be ready. The advantage of asynchronous communication is that the sender and receiver can overlap their computation because they do not wait for each other.

The reactor used in AMS handles requests delivered concurrently by multiple event resources. Mesage dispatcher handles registering and unregistering of application-defined message handlers, and dispatches messages from the demultiplexer to the associated handlers. Event demultiplexer uses an event loop to block on all resources.

#### Loosely Coupled
In a data system based on AMS, any module may be introduced into or removed from the system at any time without inhibiting the ability of any other module to continue sending and receiving messages. The system’s modules have no inter-dependency to become or stay operational. 

By publish–subscribe pattern used in AMS, senders of messages called publishers do not program the messages to be sent directly to specific receivers. Instead, published messages are characterized into classes, without knowledge of subscribers. Similarly, subscribers express interest in one or more classes, and only receive messages that are of interest, without knowledge of publishers.

#### Fault-tolerant
AMS-based systems are highly robust, lacking any single point of failure and tolerant of unplanned module termination. 

#### Dynamic Discovery
AMS provides dynamic discovery of publishers and subscribers. Dynamic discovery makes your DDS applications extensible. This means the application does not have to know or configure the endpoints for communications because they are automatically discovered. AMS will also discover the type of data being published. 

If multicast is not supported for target environment or static discovery is the preferred method, AMS uses custom config.xml file to determine peers in the network topology. 

#### Fast
Communication within an AMS-based system is rapid and efficient:
* Messages are exchanged directly between modules rather than through any central
message dispatching nexus (i.e. brokers).
* Messages are automatically conveyed using the most suitable underlying transport service (TCP/IP) to which the sending and receiving modules both have access. 

#### Scalable
Finally, AMS provides high scalability; hundreds or thousands of cooperating modules have no significant impact on application performance.

API
---

Create or just return a singleton IService instance

    static IService& IService::instance();

Create (or joins to) a messaging domain that is restricted for communication

    void IService::create_domain(std::string domainName, std::string appName);
    
Start/stop the reactor for both the communication and the dynamic discovery

    void IService::reactor_start();
    void IService::reactor_stop();

Create a subscriber for T-typed messages

    template<typename T>
    void IService::create_subscriber();
    
Subscribe a handler object for T-typed messages received

    template<typename T>
    void IService::subscribe(IHandler& handler);
    
Unsubscribe from receiving T-typed messages

    template<typename T>
    void IService::unsubscribe();

Create a publisher for T-typed messages

    template<typename T>
    void IService::create_publisher();

Send a message to all subscribers

    void IService::send_message(IMsgObj& obj);

Register a notifier for peer status updates within the domain

    void IService::register_discovery(IPeerNotification* notifier);

Return the own host ip address

    std::string IService::get_host_ip() const;
    
Destroy the service singleton instance

    static void IService::destroy();
    
Return the service global logger

    Poco::Logger& IService::logger();  

Run the service in debug mode for exhaustive logging

    void IService::debug_mode();

Dependencies
------------

* Poco C++ 1.4.x
* ZeroMQ 3.2.x
* MessagePack 0.5.x

Example
-------

Declaring our message class first. Classes can be composed from other classes.

    class TestMsg : public AMS::IMsgObj {
    public:
        // give unique id to each message
        TestMsg() : IMsgObj(/*msg id*/1) {
        }

        // can use primitive types and string
        std::string name;
        double value;
        // can use other class objects
        AdditionalInfo info;
        // can use vector and map containers
        std::vector<int> members;
        std::map<int, int> mappings;

        // allow serialization of only the selected members
        MSGPACK_DEFINE(name, value, info, members, mappings);
    };

    class AdditionalInfo {
    public:
        std::string address;
        double offset;

        // add only fields that will be serialized
        MSGPACK_DEFINE(address);
    };

Publisher example
    
    void pub() {
        // create or get the singleton AMS service
        AMS::IService& service = AMS::IService::instance();
        
        // enable exhaustive logging (i.e. message received/sent)
        service.debug_mode();
        // can use global logger of AMS service
        service.logger().information("publisher side running...");

        // should first create or join to a domain 
        // give unique domain name and application name as parameters
        service.create_domain("ams_test", "Test_PUB");

        // creates a publisher for the associated message
        service.create_publisher<TestMsg>();

        // should start the reactor for communication
        service.reactor_start();

        for (int i=0; i<100; ++i)
        {       
            // prepare a message to send
            TestMsg msg;
            msg.value = i;
            msg.name = "testing.";
            service.send_message(msg);
        }
    }
    
Subscriber example

    // define a handler class for the message first
    class TestMsgHandler : public AMS::IHandler {
    public:
        virtual void handle(AMS::IMsgObj* baseMsg) {
            TestMsg* msg = dynamic_cast<TestMsg*>(baseMsg);
            if (msg != 0) {
                // process message here
            }
        }
    };

    void sub() {
        AMS::IService& service = AMS::IService::instance();

        service.debug_mode();
        service.logger().information("subscriber side running...");
 
        // joins to the domain with unique application name
        service.create_domain("ams_test", "Test_SUB");

        // creates a subscriber for the associated message
        service.create_subscriber<TestMsg>();
        TestMsgHandler handler;
        service.subscribe<TestMsg>(handler);

        // should start the reactor for communication
        service.reactor_start();
        
        // wait enough here to receive messages sent by pusblisher
        // i.e. sleep() here
    }


License
-------

Licensed under [the Apache 2.0 license](LICENSE). 


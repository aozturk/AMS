AMS - Asynchronous Messaging Service
===

Features
--------

##### Asynchronous
Asynchronous message passing systems deliver a message from sender to receiver, without waiting for the receiver to be ready. The advantage of asynchronous communication is that the sender and receiver can overlap their computation because they do not wait for each other.

The reactor used in AMS handles requests delivered concurrently by multiple event resources. Mesage dispatcher handles registering and unregistering of application-defined message handlers, and dispatches messages from the demultiplexer to the associated handlers. Event demultiplexer uses an event loop to block on all resources.

##### Loosely Coupled
In a data system based on AMS, any module may be introduced into or removed from the system 
at any time without inhibiting the ability of any other module to continue sending and 
receiving messages. The system’s modules have no inter-dependency to become or stay operational. 

##### Fault-tolerant
AMS-based systems are highly robust, lacking any single point of failure and tolerant
of unplanned module termination. 

##### Fast
Communication within an AMS-based system is rapid and efficient:
* Messages are exchanged directly between modules rather than through any central
message dispatching nexus (i.e. brokers).
* Messages are automatically conveyed using the most suitable underlying transport
service (TCP/IP) to which the sending and receiving modules both have access. 

##### Scalable
Finally, AMS provides high scalability; hundreds or thousands of cooperating modules have
no significant impact on application performance.

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

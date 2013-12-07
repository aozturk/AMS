AMS
===

Asynchronous Messaging Service Framework

In a data system based on AMS, any module may be introduced into or removed from the system 
at any time without inhibiting the ability of any other module to continue sending and 
receiving messages. The system’s modules have no inter-dependency to become or stay operational. 

AMS-based systems are highly robust, lacking any single point of failure and tolerant
of unplanned module termination. At the same time, communication within an AMS-based
system is rapid and efficient:
* Messages are exchanged directly between modules rather than through any central
message dispatching nexus (i.e. brokers).
* Messages are automatically conveyed using the most suitable underlying transport
service to which the sending and receiving modules both have access (TCP/IP). 

Finally, AMS provides high scalability; hundreds or thousands of cooperating modules have
no significant impact on application performance.

Asynchronous Messaging Service API:

    // Create or just return a singleton instance
    static IService& instance();
    // Destroy the singleton instance
    static void destroy();

    // Create a messaging domain restricted for communication
    void create_domain(std::string domainName, std::string selfDesc);

    // Create a subscriber for T-typed messages
    template<typename T>
    void create_subscriber();

    // Create a publisher for T-typed messages
    template<typename T>
    void create_publisher();

    // Subscribe a handler to T-typed messages dispatched automatically
    template<typename T>
    void subscribe(IHandler& handler);

    // Unsubscribe from receiving T-typed messages
    template<typename T>
    void unsubscribe();

    // Send a message to all subscribers
    void send_message(IMsgObj& obj);

    // Start/stop the reactor for the communication
    void reactor_start();
    void reactor_stop();

    // Register a notifier for peer status updates within the domain
    void register_discovery(IPeerNotification* notifier);

    // Return the host ip address
    std::string get_host_ip() const;

    // Return the service logger
    Poco::Logger& logger() { return *m_consoleLogger; }        

    // Run service in debug mode
    void debug_mode() { m_consoleLogger->setLevel("debug"); }

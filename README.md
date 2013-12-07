AMS
===

Asynchronous Messaging Service Framework


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

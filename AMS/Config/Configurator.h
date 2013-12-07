#pragma once

#include "AMS/Types.h"

namespace AMS {
    class Configurator {
    public:
#define MAX_APP_ON_SAME_HOST 4

        static int getPort(int domainId) {
            return getMcastPort(domainId) + 11;
        }

        static std::string resolveHostIP() {
            Poco::Net::HostEntry host(Poco::Net::DNS::thisHost());

            const Poco::Net::HostEntry::AddressList& addressList = host.addresses();

            if (addressList.size() > 0) {
                return addressList[0].toString();
            } else {
                printf("warning: no network interface found! working on localhost\n");
                return "localhost";
            }
        }

        static std::string getMcastAddress() {
            return "239.255.0.1";
        }

        static std::string getMcastPort_str(int domainId) {
            std::stringstream st;
	        st << getMcastPort(domainId);

            return st.str();
        }

        static int getMcastPort(int domainId) {
            return portBase + domainBase * domainId;
        }

    private:
        static const int portBase = 7400;
        static const int domainBase = 250;
    };
}
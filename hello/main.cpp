#include <iostream>
#include "../lib/NearbyServer.h"

class CColorServer : public CNearby {
public:
    CColorServer(std::string localName, std::string serviceName, std::string packageName, unsigned short servicePort)
            : CNearby (localName, serviceName, packageName, servicePort) {
    };

    virtual bool onConnectionRequest(const std::string& remoteDevice, const std::string& remoteEndpoint,
                           const std::vector<uint8_t>& requestPayload, std::vector<uint8_t>& acceptPayload) override {

        std::cout << "-> Connection request from " << remoteDevice << " (" << remoteEndpoint << ")" << std::endl;

        printMessage(requestPayload, true);

        //accept anyone
        std::string reply = "Welcome " + remoteDevice;
        acceptPayload.assign(begin(reply), end(reply));

        return true;
    }

    virtual void onMessage(const std::string& remoteEndpoint, const std::vector<uint8_t>& payload, bool reliable) override {
        printMessage(payload, reliable);
    }

    virtual void onDisconnect(const std::string& remoteEndpoint) override {
        std::cout << "-> Disconnected: " << remoteEndpoint << std::endl;
    }

private :

    void printMessage(const std::vector<uint8_t>& payload, bool reliable) {

        if(!payload.empty()){
            std::string msg( payload.begin(), payload.end() );
            std::cout << "-> Message: " << msg << (reliable ? "" : "*") << std::endl;
        }

    }
};


int main() {

#ifdef __WIN32__
    CColorServer server("HolgisPC", "com.holgis.hello_service", "com.holgis.helloconnection", 37484);
#else
    CColorServer server("HolgisPi", "com.holgis.hello_service", "com.holgis.helloconnection", 37484);
#endif


    server.startAdvertising();
    server.startServer();

    //wait till finish

    server.stopAdvertising();
    server.stopServer();

    return 0;
}
#include <iostream>
#include "../lib/NearbyServer.h"

class CHelloServer : public CNearby {
public:
    CHelloServer(std::string localName, std::string serviceName, std::string packageName, unsigned short servicePort)
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

        if(!payload.empty()) {
            std::string msg(payload.begin(), payload.end());

            std::string msg2 = "You send: " + msg;
            std::vector<uint8_t> out;

            std::copy(begin(msg2), end(msg2), std::inserter(out, begin(out)));
            sendReliableMessage(remoteEndpoint, std::move(out));
        }
    }

    virtual void onDisconnect(const std::string& remoteEndpoint) override {
        std::cout << "-> Disconnected: " << remoteEndpoint << std::endl;
    }


private:

    void printMessage(const std::vector<uint8_t>& payload, bool reliable) {

        if(!payload.empty()){
            std::string msg( payload.begin(), payload.end() );
            std::cout << "-> Message: " << msg << (reliable ? "" : "*") << std::endl;
        }
    }
};


int main() {

#ifdef __WIN32__
    CHelloServer  server("HolgisPC", "com.holgis.hello_service", "com.holgis.helloconnection", 37484);
#else
    CColorServer server("HolgisPi", "com.holgis.hello_service", "com.holgis.helloconnection", 37484);
#endif

    server.startAdvertising();
    std::thread t1 = server.startServerAsync();

    std::cout << "Type x [Enter] to exit" << std::endl;

    while(true) {
        auto ch = getchar();
        if (ch == 'x') {
            break;
        }
    }

    std::cout << "Exit in 2 seconds..." << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));

    server.stopAdvertising();
    server.stopServer();

    std::cout << "Waiting for shutdown..." << std::endl;

    t1.join();

    std::cout << "Bye." << std::endl;

    return 0;
}
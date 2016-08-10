#include <iostream>
#include <thread>
#include "../lib/NearbyServer.h"
#include "../lib/NearbyMessage.h"


#ifndef __WIN32__
#include "pi/ledctrl.h"
#endif


class CColorServer : public CNearby {
public:
    CColorServer(std::string localName, std::string serviceName, std::string packageName, unsigned short servicePort)
            : CNearby (localName, serviceName, packageName, servicePort)
#ifndef __WIN32__
    ,_led(17,27,22)
#endif
    {
#ifndef __WIN32__
        _led.SetRGB(red,green,blue);
#endif
    };

    virtual bool onConnectionRequest(const std::string& remoteDevice, const std::string& remoteEndpoint,
                           const std::vector<uint8_t>& requestPayload, std::vector<uint8_t>& acceptPayload) override {

        std::cout << "-> Connection request from " << remoteDevice << " (" << remoteEndpoint << ")" << std::endl;

        acceptPayload.resize(3*4, 0);
        auto it = std::begin(acceptPayload);

        CNearbyMessage::writeLong(it, red);
        CNearbyMessage::writeLong(it, green);
        CNearbyMessage::writeLong(it, blue);

        if(red + green + blue == 0) {
            std::cout << "Setting request color" << std::endl;
            printMessage(requestPayload, true);
        } else {
            std::cout << "Keeping current color" << std::endl;
            printMessage(acceptPayload, true);
        }

        std::cout << "<- Accepting request!" << std::endl;

        return true;
    }

    virtual void onMessage(const std::string& remoteEndpoint, const std::vector<uint8_t>& payload, bool reliable) override {
        printMessage(payload, reliable);
    }

    virtual void onDisconnect(const std::string& remoteEndpoint) override {
        std::cout << "-> Disconnected: " << remoteEndpoint << std::endl;
#ifndef __WIN32__
        //to leave the LED on or not?
        //_led.SetRGB(0,0,0);
#endif
    }

private :

#ifndef __WIN32__
    CLedController _led;
#endif

    void printMessage(const std::vector<uint8_t>& payload, bool reliable) {

        if (payload.size()) {
            auto it = std::begin(payload);

            red = CNearbyMessage::readLong(it, false);
            std::advance(it, 4);
            green = CNearbyMessage::readLong(it, false);
            std::advance(it, 4);
            blue  = CNearbyMessage::readLong(it, false);
            std::advance(it, 4);

#ifndef __WIN32__
            _led.SetRGB(red,green,blue);
#else
            std::cout << "-> Message! -> RGB: (" << red << " " << green << " " << blue << ")" << (reliable ? "" : " *") << std::endl;
#endif

        }

    }
private :
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
};


int main() {

#ifdef __WIN32__
    CColorServer server("HolgisPC", "com.holgis.color_service", "com.holgis.colorconnection", 37484);
#else
    CColorServer server("HolgisPi", "com.holgis.color_service", "com.holgis.colorconnection", 37484);
#endif

    server.startAdvertising();
    std::thread t1 = server.startServerAsync();

    //wait till finish

    std::thread t2([&]() {

        std::cout << "CNearby::startup - starting timout thread!" << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(500));
        server.stopAdvertising();
        server.stopServer();

       std::cout << "CNearby::startup - session finished!" << std::endl;
    });

    t1.join();
    t2.join();

//    server.stopAdvertising();
 //   server.stopServer();

    return 0;
}
//
// Created by Holger on 17.03.2016.
//

#ifndef UNTITLED_DISCOVER_H
#define UNTITLED_DISCOVER_H


#include <string>

class IDiscover {
public:
    virtual void start_advertising(std::string packageName, std::string serviceName, unsigned short serverPort,
                                   std::string deviceName, std::string localDID, std::string localEP) = 0;
    virtual void stop_advertising() = 0;
};

class Discover {
public:
    Discover();
    virtual ~Discover();

    void start_advertising(std::string packageName, std::string serviceName, unsigned short serverPort = 37484,
                           std::string deviceName = "");
    void stop_advertising();

protected:
    std::string buildName(int nameLength, bool alphaNumeric);

private:

    std::string mLocalDID;
    std::string mLocalEP;

    IDiscover* mInstance;
};


#endif //UNTITLED_DISCOVER_H

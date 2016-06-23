//
// Created by Holger on 17.03.2016.
//

#ifndef UNTITLED_BONJOUR_H
#define UNTITLED_BONJOUR_H

#include "../Discover.h"

class Bonjour : public IDiscover {

    void start_advertising(std::string packageName, std::string serviceName, unsigned short serverPort,
                           std::string deviceName, std::string localDID, std::string localEP);
    void stop_advertising();

};


#endif //UNTITLED_BONJOUR_H

//
// Created by Holger on 17.03.2016.
//

#include "Discover.h"

#ifdef __WIN32__
#include "win32/Bonjour.h"
#else
#include "pi/Avahi.h"
#endif

#include <iostream>
#include <ctime>

Discover::Discover() {
#ifdef __WIN32__
    mInstance = new Bonjour();
#else
    mInstance = new Avahi();
#endif

    std::srand(std::time(0)); // use current time as seed for random generator

    mLocalDID = buildName(9, true);
}

Discover::~Discover() {
    delete mInstance;
}

void Discover::start_advertising(std::string packageName, std::string serviceName, unsigned short serverPort,
                                 std::string deviceName) {

    mLocalEP = buildName(9,false);
    std::cout << "Local Server ID: " << mLocalDID <<":"<< mLocalEP<< std::endl;

    if(mInstance) {
        mInstance->start_advertising(packageName, serviceName, serverPort, deviceName, mLocalDID, mLocalEP);
    }
}

void Discover::stop_advertising() {
    if(mInstance)
        mInstance->stop_advertising();
}

std::string Discover::buildName(int nameLength, bool alphaNumeric) {
    std::string name;
    for(int i=0; i<nameLength; ++i) {
        int r = std::rand()%(alphaNumeric?36:10);
        if(r<10){
            name.push_back('0'+r);
        } else {
            name.push_back('A'+r-10);
        }
    }
    return name;
}
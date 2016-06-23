//
// Created by Holger on 11.03.2016.
//

#ifndef UNTITLED_NEARBYSESSION_H
#define UNTITLED_NEARBYSESSION_H


#include <string>
#include <vector>
#include <map>

#include "socket_platform.h"

class CNearby;

class CNearbySession {
public:
    CNearbySession(CNearby* nearbyServer);
    virtual ~CNearbySession();

    void doSession(SOCKET sessionSocket, SOCKET cancelSocket = SOCKET_ERROR);

private:
    bool readRequest(SOCKET sessionSocket, std::vector<uint8_t>& buffer, uint32_t& sequence,
                     std::string& remoteDevice, std::string& remoteEndpoint);
    bool sendAnswer(SOCKET sessionSocket, const std::vector<uint8_t>& message);

    SOCKET sessionSocket = SOCKET_ERROR;
    const size_t chunkSize = 2048;

    CNearby* nearbyServer = nullptr;
};


#endif //UNTITLED_NEARBYSESSION_H

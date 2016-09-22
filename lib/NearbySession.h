//
// Created by Holger on 11.03.2016.
//

#ifndef UNTITLED_NEARBYSESSION_H
#define UNTITLED_NEARBYSESSION_H


#include <string>
#include <vector>
#include <map>
#include <mutex>

#include "socket_platform.h"

class CNearby;

class CNearbySession {
public:
    CNearbySession(CNearby* nearbyServer, const struct sockaddr* remoteAddress);
    virtual ~CNearbySession();

    void doSession(SOCKET sessionSocket, SOCKET cancelSocket = SOCKET_ERROR);

    void sendMessage(std::vector<uint8_t>&& message, bool reliable = true);

private:
    bool readRequest(SOCKET sessionSocket, std::vector<uint8_t>& buffer, uint32_t& sequence,
                     std::string& remoteDevice, std::string& remoteEndpoint);
    bool sendAnswer(SOCKET sessionSocket, const std::vector<uint8_t>& message);

    bool doWriteQueue(SOCKET sessionSocket, uint32_t& sequence);


    SOCKET sessionSocket = SOCKET_ERROR;
    SOCKET queueReadySocket = SOCKET_ERROR;

    SOCKET createLocalUDP();
    struct sockaddr remoteAddr;
    SOCKET udpSocket = SOCKET_ERROR;

    const size_t chunkSize = 2048;

    CNearby* nearbyServer = nullptr;

    std::mutex messageMutex;
    std::vector<std::vector<uint8_t>> messageQueue;
};


#endif //UNTITLED_NEARBYSESSION_H

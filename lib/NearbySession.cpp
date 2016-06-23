//
// Created by Holger on 11.03.2016.
//

#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

#include "socket_platform.h"

#include "NearbyServer.h"
#include "NearbySession.h"
#include "NearbyMessage.h"


CNearbySession::CNearbySession(CNearby* nearbyServer) {

    this->nearbyServer = nearbyServer;

}

CNearbySession::~CNearbySession() {
    if (nearbyServer && sessionSocket != SOCKET_ERROR ) {
        nearbyServer->sessionDisconnect(sessionSocket);
    }
}


void CNearbySession::doSession(SOCKET sessionSocket, SOCKET cancelSocket) {

    this->sessionSocket = sessionSocket;

    std::cout << "CNearby::startup - accepted connection!" << std::endl;

    uint32_t sequence = 1;

    fd_set readFds;

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    std::string remoteDevice, remoteEndpoint;

    SOCKET selectSocket = std::max<SOCKET>(sessionSocket, cancelSocket) + 1;

    std::vector<uint8_t> buffer;
    buffer.reserve(chunkSize*2);

    bool sessionLoop = true;

    while(sessionLoop) {

        FD_ZERO(&readFds);

        FD_SET(sessionSocket, &readFds);
        if (cancelSocket != SOCKET_ERROR) {
            FD_SET(cancelSocket, &readFds);
        }

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        if(int activity = select(selectSocket, &readFds, nullptr, nullptr, &tv)) {

            //check for read
            if (FD_ISSET(cancelSocket, &readFds)) {
                //data on cancelSocket means we should quit
                std::cerr << "CNearby::doSession - sessionSocket error set!" << std::endl;
                break;
            }

            if (FD_ISSET(sessionSocket, &readFds)) {
                sessionLoop = readRequest(sessionSocket, buffer, sequence, remoteDevice, remoteEndpoint);
            }
        }
        else
        {
            std::cerr << "CNearby::doSession - send Ping!" << std::endl;

            CNearbyMessage message;
            sendAnswer(sessionSocket, message.buildPing(sequence));
        }
    }

    closesocket(sessionSocket);
    this->sessionSocket = SOCKET_ERROR;

    std::cout << "CNearby::doSession - complete!" << std::endl;
}

bool CNearbySession::readRequest(SOCKET sessionSocket, std::vector<uint8_t>& buffer, uint32_t& sequence, std::string& remoteDevice, std::string& remoteEndpoint){

    bool sessionLoop = true;

    struct sockaddr_in cliAddr;
    socklen_t cliLen = sizeof(cliAddr);

    size_t dataSize = buffer.size();
    buffer.resize(dataSize + chunkSize);

    if (int read = recvfrom(sessionSocket, (char *) &buffer[dataSize], chunkSize, 0,
                                   (struct sockaddr *) &cliAddr, &cliLen)) {

        buffer.resize(dataSize + read);

        CNearbyMessage message;
        while (auto readMsg = message.parse(buffer)) {

            buffer.erase(std::begin(buffer), std::next(std::begin(buffer), readMsg)); //remove message from buffer

            if (message.wantAck()) {
                sendAnswer(sessionSocket, message.buildAck());
            }

            if (message.isRequest()) {

                std::string local;
                //TODO check if local is correct

                std::vector<uint8_t> requestPayload;
                message.getRequestPayload(remoteDevice, remoteEndpoint, local, requestPayload);

                std::vector<uint8_t> acceptPayload;
                if(nearbyServer->sessionRequest(sessionSocket, remoteDevice, remoteEndpoint, requestPayload, acceptPayload)) {
                    sendAnswer(sessionSocket, message.buildAccept(sequence++, acceptPayload));
                }

            } else if (message.isMessage()) {

                std::vector<uint8_t> payload;
                message.getMessagePayload(payload);

                nearbyServer->sessionMessage(sessionSocket, payload, message.isReliable());

            } else if (message.isQuit()) {

                nearbyServer->sessionDisconnect(sessionSocket);

                sessionLoop = false;
            }
        }
    }

    return sessionLoop;
}

bool CNearbySession::sendAnswer(SOCKET sessionSocket, const std::vector<uint8_t> &message) {

    auto size = CNearbyMessage::writeLong(message.size());

    int wrote = send(sessionSocket, (char *) &size[0], size.size(), 0);
    wrote += send(sessionSocket, (char *) &message[0], message.size(), 0);

    return wrote == message.size() + size.size();
}

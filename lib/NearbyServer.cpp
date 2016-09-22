//
// Created by Holger on 11.03.2016.
//

#include <iostream>
#include <iomanip>
#include <chrono>

#include "NearbyServer.h"
#include "NearbySession.h"
#include "NearbyMessage.h"
#include "NotificationSocket.h"


CNearby::CNearby(const std::string& name, const std::string& service, const std::string& package, unsigned short port) {

    localName = name;
    packageName = package;
    serviceName = service;
    serverPort = port;

#ifdef __WIN32__
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    cancelSocket = NotificationSocket::Create();

}

CNearby::~CNearby() {
#ifdef __WIN32__
    WSACleanup();
#endif
}


bool CNearby::serverLoop = true;

void CNearby::startAdvertising() {

    std::cout << "CNearby::startup - start advertising" << std::endl;
    discover.start_advertising(packageName, serviceName, serverPort, localName);
}

void CNearby::stopAdvertising() {

    discover.stop_advertising();
}


std::thread CNearby::startServerAsync() {
    return std::thread([&]() {
        startServer();
    });
}

bool CNearby::startServer() {

    std::cout << "CNearby::startup - creating server socket" << std::endl;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servAddr, cliAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);;
    servAddr.sin_port = htons(serverPort);

    std::cout << "CNearby::startup - binding..." << std::endl;
    int err = bind(serverSocket, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if (err < 0) {
        std::cerr << "CNearby::startup - error binding socket" << std::endl;
        return false;
    }

    std::cout << "CNearby::startup - listening..." << std::endl;

    err = listen(serverSocket, 5);
    if (err < 0) {
        std::cerr << "CNearby::startup - error listening socket" << std::endl;
        return false;
    }

    std::vector<std::thread> sessions;

    SOCKET udpSocket = createUDPSocket(serverPort);
    if (udpSocket < 0) {
        std::cerr << "CNearby::startup - error creating udp socket" << std::endl;
    }

    sessions.emplace_back([=]() {
        doUDPSession(udpSocket, cancelSocket);
        std::cout << "CNearby::startup - udp sessionfinished!" << std::endl;
    });

    std::cout << "CNearby::startup - start accepting..." << std::endl;

    serverLoop = true;
    while(serverLoop) {

        std::cout << "CNearby::startup - in accepting loop..." << std::endl;

        fd_set readFds;

        struct timeval tv;
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        SOCKET selectSocket = std::max<SOCKET>(serverSocket, cancelSocket) + 1;

        FD_ZERO(&readFds);

        FD_SET(serverSocket, &readFds);
        if (cancelSocket != SOCKET_ERROR) {
            FD_SET(cancelSocket, &readFds);
        }

        if(int activity = select(selectSocket, &readFds, nullptr, nullptr, &tv)) {

            if (FD_ISSET(serverSocket, &readFds)) {

                socklen_t cliLen = sizeof(cliAddr);
                SOCKET sessionSocket = accept(serverSocket, (struct sockaddr *) &cliAddr, &cliLen);
                if (sessionSocket < 0) {
                    std::cout << "CNearby::startup - error accepting socket" << std::endl;
                    continue;
                }

                socketLookup[cliAddr.sin_addr.s_addr] = sessionSocket;

                CNearbySession *nearbySession = new CNearbySession(this, (const struct sockaddr*)&cliAddr);
                {
                    std::lock_guard<std::mutex> lock(sessionLookupMutex);
                    sessionLookup.insert(std::make_pair(sessionSocket, nearbySession));
                }

                std::cout << "CNearby::startup - trying to start session..." << std::endl;

                //starting new thread for session
                sessions.emplace_back([=]() {
                    nearbySession->doSession(sessionSocket, cancelSocket);

                    {
                        std::lock_guard<std::mutex> lock(sessionLookupMutex);
                        auto it = sessionLookup.find(sessionSocket);
                        if(it != sessionLookup.end()) {
                            sessionLookup.erase(it);
                        }
                    }

                    delete nearbySession;
                    std::cout << "CNearby::startup - session finished!" << std::endl;

                });

            } else {
                std::cout << "CNearby::startup - shutdown, cancelSocket was called!" << std::endl;
            }
        }

    }

    closesocket(serverSocket);

    for(auto& s: sessions) {
        s.join();
    }

    return true;

}

void CNearby::stopServer() {

    serverLoop = false;
    NotificationSocket::Notify(cancelSocket);
}

bool CNearby::onConnectionRequest(const std::string& remoteDevice, const std::string& remoteEndpoint,
                              const std::vector<uint8_t>& requestPayload, std::vector<uint8_t>& acceptPayload) {

    return true;
}

void CNearby::onMessage(const std::string& remoteEndpoint, const std::vector<uint8_t>& payload, bool reliable) {

}

void CNearby::onDisconnect(const std::string& remoteEndpoint) {

}

void CNearby::sendReliableMessage(const std::string& remoteEndpoint, std::vector<uint8_t>&& payload) {

    for(auto& sc: sessionContext) {
        if(sc.second.remoteEndpoint == remoteEndpoint){
            std::lock_guard<std::mutex> lock(sessionLookupMutex);
            auto it = sessionLookup.find(sc.first);
            if(it != sessionLookup.end()){
                it->second->sendMessage(std::move(payload), true);
            }
        }
    }
}

void CNearby::sendUnreliableMessage(const std::string& remoteEndpoint, std::vector<uint8_t>&& payload) {

    for(auto& sc: sessionContext) {
        if(sc.second.remoteEndpoint == remoteEndpoint){
            std::lock_guard<std::mutex> lock(sessionLookupMutex);
            auto it = sessionLookup.find(sc.first);
            if(it != sessionLookup.end()){
                it->second->sendMessage(std::move(payload), false);
            }
        }
    }
}

bool CNearby::sessionRequest(SOCKET sessionSocket, const std::string& remoteDevice, const std::string& remoteEndpoint,
                                   const std::vector<uint8_t>& requestPayload, std::vector<uint8_t>& acceptPayload) {

    if(onConnectionRequest(remoteDevice, remoteEndpoint, requestPayload, acceptPayload)) {

        struct sSessionContext context;
        context.remoteDevice = remoteDevice;
        context.remoteEndpoint = remoteEndpoint;

        sessionContext[sessionSocket] = context;
        return true;
    }

    return false;
}

void CNearby::sessionMessage(SOCKET sessionSocket, const std::vector<uint8_t>& payload, bool reliable) {

    auto it = sessionContext.find(sessionSocket);

    if(it != sessionContext.end()){
        onMessage(it->second.remoteEndpoint, payload, reliable);
    }
}

void CNearby::sessionDisconnect(SOCKET sessionSocket) {

    for(auto i1 = socketLookup.begin(); i1 != socketLookup.end(); ++i1) {
        if(i1->second == sessionSocket) {
            socketLookup.erase(i1);
            break;
        }
    }

    auto i2 = sessionContext.find(sessionSocket);
    if(i2 != sessionContext.end()){
        onDisconnect(i2->second.remoteEndpoint);
        sessionContext.erase(sessionSocket);
    }
}

SOCKET CNearby::createUDPSocket(unsigned short serverPort) {

    SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(serverPort);

    if( bind(serverSocket , (struct sockaddr*)&servAddr, sizeof(servAddr) ) >= 0) {

        return serverSocket;
    }

    return -1;
}


void CNearby::doUDPSession(SOCKET udpSocket, SOCKET cancelSocket) {

    std::cout << "CNearby::startup - accepted connection!" << std::endl;

    uint32_t sequence = 1;

    fd_set readFds;

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    std::string remoteDevice, remoteEndpoint;

#ifdef __WIN32__
    SOCKET selectSocket = udpSocket;
#else
    SOCKET selectSocket = std::max<int>(udpSocket, cancelSocket) + 1;
#endif

    std::vector<uint8_t> buffer;
    buffer.reserve(chunkSize*2);

    bool sessionLoop = true;

    while(sessionLoop) {

        FD_ZERO(&readFds);

        FD_SET(udpSocket, &readFds);
        if(cancelSocket!=SOCKET_ERROR) {
            FD_SET(cancelSocket, &readFds);
        }

        tv.tv_sec = 5;
        tv.tv_usec = 0;

        if(int activity = select(selectSocket, &readFds, nullptr, nullptr, &tv)) {

            if (FD_ISSET(cancelSocket, &readFds)) {
                std::cerr << "CNearby::doSession - cancelSocket read set!" << std::endl;
                break;
            }

            if (FD_ISSET(udpSocket, &readFds)) {
                readUDPSocket(udpSocket, buffer, sequence, remoteDevice, remoteEndpoint);
            }
        }
    }

    closesocket(udpSocket);

    std::cout << "CNearby::doSession - complete!" << std::endl;
}

void CNearby::readUDPSocket(SOCKET readSocket, std::vector<uint8_t>& buffer, uint32_t& sequence, std::string& remoteDevice, std::string& remoteEndpoint) {

    struct sockaddr_in cliAddr;
    socklen_t cliLen = sizeof(cliAddr);

    size_t dataSize = buffer.size();
    buffer.resize(dataSize + chunkSize);

    if (int read = recvfrom(readSocket, (char *) &buffer[dataSize], chunkSize, 0,
                                   (struct sockaddr *) &cliAddr, &cliLen)) {

        buffer.resize(dataSize + read);

        CNearbyMessage message;
        while (auto readMsg = message.parse(buffer)) {

            buffer.erase(std::begin(buffer), std::next(std::begin(buffer), readMsg)); //remove message from buffer

            if (message.isMessage()) {

                auto it = socketLookup.find(cliAddr.sin_addr.s_addr);
                if(it != socketLookup.end()) {

                    auto i2 = sessionContext.find(it->second);
                    if(i2 != sessionContext.end()) {

                        std::vector<uint8_t> payload;
                        message.getMessagePayload(payload);

                        onMessage(i2->second.remoteEndpoint, payload, message.isReliable());
                    }
                }
            }
        }
    }
}

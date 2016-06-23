//
// Created by Holger on 11.03.2016.
//

#ifndef UNTITLED_NEARBYSERVER_H
#define UNTITLED_NEARBYSERVER_H

#include <string>
#include <vector>
#include <map>

#include "Discover.h"
#include "socket_platform.h"


class CNearby {

public:
    CNearby(const std::string& localName, const std::string& serviceName, const std::string& packageName, unsigned short servicePort);
    virtual ~CNearby();

    void startAdvertising();
    void stopAdvertising();

    bool startServer();
    void stopServer();

    virtual bool onConnectionRequest(const std::string& remoteDevice, const std::string& remoteEndpoint,
                           const std::vector<uint8_t>& requestPayload, std::vector<uint8_t>& acceptPayload);
    virtual void onMessage(const std::string& remoteEndpoint, const std::vector<uint8_t>& payload, bool reliable);
    virtual void onDisconnect(const std::string& remoteEndpoint);

    //void sendReliableMessage(const std::string& remoteEndpoint, const std::vector<uint8_t>& payload);
    //void sendReliableMessage(const std::vector<std::string>& remoteEndpoints, const std::vector<uint8_t>& payload);
    //void sendUnreliableMessage(const std::string& remoteEndpoint, const std::vector<uint8_t>& payload);
    //void sendUnreliableMessage(const std::vector<std::string>& remoteEndpoints, const std::vector<uint8_t>& payload);

    //void startDiscovery();
    //void stopDiscovery();
    //void onEndpointFound(const std::string& endpointID, const std::string& deviceID, const std::string& serviceID, const std::string& remoteName);
    //void onEndpointLost(const std::string& endpointID);

    //void sendConnectionRequest(const std::string& remoteEndpoint, const std::vector<uint8_t>& payload);
    //void DisconnectFrom(const std::string& remoteEndpoint);

private:
    friend class CNearbySession;
    bool sessionRequest(SOCKET sessionSocket, const std::string& remoteDevice, const std::string& remoteEndpoint,
                        const std::vector<uint8_t>& requestPayload, std::vector<uint8_t>& acceptPayload);
    void sessionMessage(SOCKET sessionSocket, const std::vector<uint8_t>& payload, bool reliable);
    void sessionDisconnect(SOCKET sessionSocket);

private:
    SOCKET createUDPSocket(unsigned short serverPort);
    void doUDPSession(SOCKET udpSocket, SOCKET cancelSocket = SOCKET_ERROR);
    void readUDPSocket(SOCKET readSocket, std::vector<uint8_t>& buffer, uint32_t& sequence,
                       std::string& remoteDevice, std::string& remoteEndpoint);

    struct sSessionContext{
        std::string remoteDevice;
        std::string remoteEndpoint;
    };

    std::map<SOCKET, struct sSessionContext> sessionContext;
    std::map<u_long, SOCKET> socketLookup;

    SOCKET serverSocket;
    static bool serverLoop;

    const size_t chunkSize = 2048;

    std::string localName;
    std::string serviceName;
    std::string packageName;
    unsigned short serverPort = 0;

    Discover discover;

};


#endif //UNTITLED_NEARBYSERVER_H

//
// Created by Holger on 08.08.2016.
//

#include "NotificationSocket.h"


SOCKET NotificationSocket::Create() {

    if(SOCKET loopSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {
        struct sockaddr_in loop_addr, local_addr;

        loop_addr.sin_family = AF_INET;
        loop_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        loop_addr.sin_port = htons(0);

       if(bind(loopSocket , (struct sockaddr*)&loop_addr, sizeof(loop_addr) ) >= 0) {

            socklen_t len = sizeof(local_addr);
            if(getsockname(loopSocket, (struct sockaddr*) &local_addr, &len) >= 0) {

                if(connect(loopSocket, (struct sockaddr*) &local_addr, sizeof(local_addr)) >= 0) {
                    return loopSocket;
                }
            }
        }
        closesocket(loopSocket);
    }

    return INVALID_SOCKET;
}

void NotificationSocket::Notify(SOCKET s) {
    char buf = 'x';
    send(s, &buf, sizeof(buf), 0);
}

void NotificationSocket::Clear(SOCKET s) {
    char buf = 0;

    struct sockaddr_in adr;
    socklen_t len = sizeof(adr);

    if(doubleCheck(s)) {
        int read = recvfrom(s, (char *) &buf, sizeof(buf), 0, (struct sockaddr *) &adr, &len);
    }
}

bool NotificationSocket::doubleCheck(SOCKET s) {

    fd_set readFds;
    FD_ZERO(&readFds);

    FD_SET(s, &readFds);

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1;

    if(int activity = select(s + 1, &readFds, nullptr, nullptr, &tv)) {

        if(FD_ISSET(s, &readFds)) {
            return true;
        }
    }

    return false;
}
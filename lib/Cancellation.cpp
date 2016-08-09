//
// Created by Holger on 08.08.2016.
//

#include "Cancellation.h"



SOCKET Cancellation::Create()
{

    if(SOCKET loopSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) {
        struct sockaddr_in loop_addr, local_addr;

        loop_addr.sin_family = AF_INET;
        loop_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        loop_addr.sin_port = htons(0);
int err =0;

       if(err = bind(loopSocket , (struct sockaddr*)&loop_addr, sizeof(loop_addr) ) >= 0) {

            int len = sizeof(local_addr);
            if(err = getsockname(loopSocket, (struct sockaddr*) &local_addr, &len) >= 0) {

                if(err= connect(loopSocket, (struct sockaddr*) &local_addr, sizeof(local_addr)) >= 0) {
                    return loopSocket;
                }
                else{

                    err = WSAGetLastError();
                    err = err;
                }
            }
        }
        closesocket(loopSocket);
    }

    return INVALID_SOCKET;
}

void Cancellation::Cancel(SOCKET cancel)
{
    char buf = 0;
    send(cancel, &buf, sizeof(buf), 0);
}

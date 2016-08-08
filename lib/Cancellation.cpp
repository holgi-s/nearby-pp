//
// Created by Holger on 08.08.2016.
//

#include "Cancellation.h"



SOCKET Cancellation::Create()
{
    SOCKET loop = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(loop != INVALID_SOCKET ) {

        struct sockaddr_in loop_addr = {}, local_addr = {};

        loop_addr.sin_family = AF_INET;
        loop_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        loop_addr.sin_port = htons(0);

        if(SOCKET_ERROR != bind(loop, (struct sockaddr*) &local_addr, sizeof(local_addr)))
        {
            int len = sizeof(local_addr);
            if(SOCKET_ERROR != getsockname(loop, (struct sockaddr*) &local_addr, &len))
            {
                if(SOCKET_ERROR != connect(loop, (struct sockaddr*) &local_addr, sizeof(len)))
                {
                    return loop;
                }
            }
        }

        closesocket(loop);
    }

    return INVALID_SOCKET;
}

void Cancellation::Cancel(SOCKET cancel)
{
    char buf = 0;
    send(cancel, &buf, sizeof(buf), 0);
}

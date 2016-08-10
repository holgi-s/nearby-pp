//
// Created by Holger on 08.08.2016.
//

#ifndef PROJECT_CANCELATION_H
#define PROJECT_CANCELATION_H

#include "socket_platform.h"

class Cancellation {

public:
    static SOCKET Create();
    static void Cancel(SOCKET cancel);
    static void Clear(SOCKET cancel);
};


#endif //PROJECT_CANCELATION_H

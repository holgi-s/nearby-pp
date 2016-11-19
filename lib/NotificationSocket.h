//
// Created by Holger on 08.08.2016.
//

#ifndef PROJECT_CANCELATION_H
#define PROJECT_CANCELATION_H

#include "socket_platform.h"

/**
 * The NotificationSocket can be used to send notificateions across threads
 *
 */

class NotificationSocket {

public:

    /**
     * Creats a local udp socket that can be used to wake up blocking socket function
     * @return notification socket
     */
    static SOCKET Create();

    /**
     * Send notification to wake up the blocking socket function
     * it is done by sending one byte on this socket
     * @param s notification socket
     */
    static void Notify(SOCKET s);

    /**
     * Clear / Acknoledge the notification
     * it is done by reading one byte from this socket
     * @param s
     */
    static void Clear(SOCKET s);

private:

    /**
     * double check id there is still a notification pending, in case multiple selects are listening for a notification
     * @param s
     * @return true is read data is still available
     */
    static bool doubleCheck(SOCKET s);

};


#endif //PROJECT_CANCELATION_H

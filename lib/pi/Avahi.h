//
// Created by Holger on 17.03.2016.
//




#ifndef UNTITLED_CAVAHI_H
#define UNTITLED_CAVAHI_H

#include "../Discover.h"

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>

class Avahi : public IDiscover {
public:
    Avahi();
    virtual ~Avahi();

    void start_advertising(std::string packageName, std::string serviceName, unsigned short serverPort,
                           std::string deviceName, std::string localDID, std::string localEP);
    void stop_advertising();


private:

    static void client_callback(AvahiClient *c, AvahiClientState state, AVAHI_GCC_UNUSED void * userdata);
    void client_callback(AvahiClient *c, AvahiClientState state);

    static void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata);
    void entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state);

    void create_services(AvahiClient *c) ;
    void collision(AvahiClient *c) ;
    void fail();

    AvahiSimplePoll* simple_poll = nullptr;
    AvahiClient* client = nullptr;
    AvahiEntryGroup *group = nullptr;
    char* name = nullptr;

    std::string packageName;
    std::string serviceName;
    unsigned short serverPort;
    std::string deviceName;
    std::string localDID;
    std::string localEP;

};


#endif //UNTITLED_CAVAHI_H



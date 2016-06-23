//
// Created by Holger on 17.03.2016.
//


#include "Avahi.h"

#include <avahi-client/client.h>
#include <avahi-client/publish.h>

#include <avahi-common/alternative.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>
#include <iostream>

Avahi::Avahi()
{
    simple_poll = avahi_simple_poll_new();
}

Avahi::~Avahi()
{
    if (simple_poll) {
        avahi_simple_poll_free(simple_poll);
    }
}

//static
void Avahi::client_callback(AvahiClient *c, AvahiClientState state, void * userdata) {
    if(auto me = static_cast<Avahi*>(userdata)) {
        me->client_callback(c, state);
    }
}

void Avahi::client_callback(AvahiClient *c, AvahiClientState state) {
    if(c){
        switch (state) {
            case AVAHI_CLIENT_S_RUNNING:
                create_services(c);
                break;
            case AVAHI_CLIENT_FAILURE:
                std::cerr << "Client failure " << avahi_strerror(avahi_client_errno(c)) << std::endl;
                avahi_simple_poll_quit(simple_poll);
                break;
            case AVAHI_CLIENT_S_COLLISION:
                std::cerr << "Client failure " << "AVAHI_CLIENT_S_COLLISION" << std::endl;
                //no break
            case AVAHI_CLIENT_S_REGISTERING:
                if (group) {
                    avahi_entry_group_reset(group);
                }
                break;
            case AVAHI_CLIENT_CONNECTING:
                ;
        }
    }
}

void Avahi::entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata) {
    if(auto me = static_cast<Avahi*>(userdata)) {
        me->entry_group_callback(g, state);
    }
}

void Avahi::entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state) {

    assert(g == group || group == NULL);
    group = g;

    switch (state) {
        case AVAHI_ENTRY_GROUP_ESTABLISHED :
            std::cerr << "Service " << name << " successfully established." << std::endl;
            break;

        case AVAHI_ENTRY_GROUP_COLLISION : {
            char* n = avahi_alternative_service_name(name);
            avahi_free(name);
            name = n;

            std::cerr << "Service name collision, renaming service to " <<  name << std::endl;

            create_services(avahi_entry_group_get_client(g));
            break;
        }

        case AVAHI_ENTRY_GROUP_FAILURE :
            std::cerr << "Entry group failure: " << avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g)))
                << std::endl;

            avahi_simple_poll_quit(simple_poll);
            break;

        case AVAHI_ENTRY_GROUP_UNCOMMITED:
        case AVAHI_ENTRY_GROUP_REGISTERING:
            ;
    }
}

void Avahi::create_services(AvahiClient *c) {

    if(c) {

        if (!group) {
            if (!(group = avahi_entry_group_new(c, entry_group_callback, (void*)this))) {
                std::cerr << "avahi_entry_group_new() failed: " << avahi_strerror(avahi_client_errno(c)) << std::endl;
                return fail();
            }
        }

        if (avahi_entry_group_is_empty(group)) {

            std::cerr << "Adding service " << name << std::endl;

            std::string id = "id=" + localDID + ":" + localEP;
            std::string si = "si=" + serviceName;
            std::string ac = "ac=1";
            std::string p10 = "p10=" + packageName;

            AvahiStringList* txt = avahi_string_list_new(NULL);
            avahi_string_list_add(txt, id.c_str());
            avahi_string_list_add(txt, si.c_str());
            avahi_string_list_add(txt, ac.c_str());
            avahi_string_list_add(txt, p10.c_str());

            int ret = 0;
            if ((ret = avahi_entry_group_add_service(
                group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, (AvahiPublishFlags)0,
                name, "_googlenearby._tcp", NULL, NULL, serverPort,
                id.c_str(), si.c_str(), ac.c_str(), p10.c_str(), NULL)) < 0) {

                if (ret == AVAHI_ERR_COLLISION)
                    return collision(c);

                std::cerr << "Failed to add service: " << avahi_strerror(ret) << std::endl;

                avahi_string_list_free (txt);
                return fail();
            }
            avahi_string_list_free (txt);

            if ((ret = avahi_entry_group_commit(group)) < 0) {
                std::cerr << "Failed to commit entry group: " << avahi_strerror(ret) << std::endl;
                return fail();
            }
        }
    }
}

void Avahi::collision(AvahiClient *c) {

    std::cerr << "Service name collision, renaming service to " << name << std::endl;

    char* n = avahi_alternative_service_name(name);
    avahi_free(name);
    name = n;

    avahi_entry_group_reset(group);

    create_services(c);
}

void Avahi::fail() {
    avahi_simple_poll_quit(simple_poll);
}

void Avahi::start_advertising(std::string _packageName, std::string _serviceName, unsigned short _serverPort,
                              std::string _deviceName, std::string _localDID, std::string _localEP) {

    packageName = _packageName;
    serviceName = _serviceName;
    serverPort = _serverPort;
    deviceName = _deviceName;
    localDID = _localDID;
    localEP = _localEP;

    name = avahi_strdup(deviceName.c_str());

    int ret = 0;
    if(!(client = avahi_client_new(avahi_simple_poll_get(simple_poll), (AvahiClientFlags)0, client_callback, (void*)this, &ret))){
      std::cerr << "Failed to create client: " << avahi_strerror(ret) << std::endl;
    }
}

void Avahi::stop_advertising() {
    if (client) {
        avahi_client_free(client);
    }
    if(name){
        avahi_free(name);
    }
}

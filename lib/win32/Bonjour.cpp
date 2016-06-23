//
// Created by Holger on 17.03.2016.
//

#include <vector>
#include "Bonjour.h"

void Bonjour::start_advertising(std::string packageName, std::string serviceName, unsigned short serverPort,
                                std::string deviceName, std::string localDID, std::string localEP)
{

    std::string si = "si=" + serviceName;
    std::string p10 = "p10=" + packageName;
    std::string id = "id=" + localDID + ":" + localEP;
    std::string port = std::to_string(serverPort);

    std::string exe = "C:\\Program Files\\Bonjour\\dns_sd64.exe";

    std::vector<const char*> params;

    params.push_back(exe.c_str());

    params.push_back("-R");
    params.push_back(deviceName.c_str());
    params.push_back("_googlenearby._tcp.");
    params.push_back(".");
    params.push_back(port.c_str());
    params.push_back(id.c_str());
    params.push_back(si.c_str());
    params.push_back("ac=1");
    params.push_back(p10.c_str());
    params.push_back(nullptr);

    spawnv(P_NOWAIT, exe.c_str(), (char* const*)&params[0]);

}

void Bonjour::stop_advertising()
{
}


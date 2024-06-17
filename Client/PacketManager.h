#pragma once

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/ioctl.h>
#include <net/if.h>

#include "Message.h"

class PacketManager {
private:
    ifreq MTU;
    int windowSize = 0;

public:
    Message* requestMessage = new Message();
    Message* responseMessage = new Message();

    Packet* prepareData(std::string message, int destinationPort, int sn);
    
    void setWindowSize(int ws);
    void setPackets(std::string& payload, const sockaddr_in& serverAddr);

    bool checkChecksum(Packet& packet);

};
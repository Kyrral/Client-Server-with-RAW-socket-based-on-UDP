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

    Packet* prepareData(std::string message, int sourcePort, int destinationPort, int sn);
    
    void setWindowSize(int ws);
    int setMTU(int& socketfd);
    void setPackets(std::string& payload, const sockaddr_in& clientAddr, const sockaddr_in& serverAddr);
    bool checkChecksum(Packet& packet);
};
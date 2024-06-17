#pragma once

#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <chrono>
#include <atomic>

#include "PacketManager.h"
#include "Randomizer.h"

class TCPClient {
private:
    std::atomic<bool> expired = false;
    int RTT = 0;

    Randomizer rand;
    PacketManager pm;

    int socketfd;
	sockaddr_in serverAddress, clientAddr, expectedServerAddress;

    int len = sizeof(serverAddress);

    bool createSocket();
    void connect();
    void requestWindowSize();

    Packet* receiveSYNACK();
    Packet* receiveData();

    bool sendData(Packet& packet);
    bool sendSYNACK(Packet& packet);

    bool sendMessage(std::string str);

public:
    void setConnection();

    bool sendMessage();

    bool receiveMessage();
};
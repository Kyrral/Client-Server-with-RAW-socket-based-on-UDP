#include <sys/socket.h>
#include <iostream>
#include <cstring>
#include <atomic>
#include <arpa/inet.h>
#include <chrono>
#include <signal.h>

#include "PacketManager.h"
#include "Randomizer.h"

class TCPServer {
    //fileds
    int RTT = 0;

    //socket
	int socketfd;
	sockaddr_in serverAddress, clientAddress;

    //packets
    Randomizer rand;
    PacketManager* pm = new PacketManager();

    Packet* requestPacket = nullptr;

    Packet* ackRecPacket = nullptr;
    Packet* ackRespPacket = nullptr;

    //methods
    bool createSocket();
    void connect();
    void requestWindowSize();

    bool sendData(Packet& packet);
    bool sendSYNACK(Packet& packet);

    Packet* receiveData();
    Packet* receiveSYNACK();

public:
    void setConnection();

    bool sendMessage();
    bool receiveMessage();
};
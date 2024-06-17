#pragma once

#include <netinet/in.h>
#include <signal.h>
#include <cstring>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>

#include "Header.h"

class Packet {
public:
	static int BUFFER_SIZE;

	Packet() = default;
    Packet(uint16_t sp, uint16_t dp, uint16_t ws, uint16_t sn);

	Header header;
	std::string payload;
	
	uint16_t computeChecksum();

	//setters
	static void setBufSize(int size);
	void setPayload(std::string payload);

	void deserialize(char* buffer, int bytes_received);

	//getters
	int packetSize();
	std::string convertToString();
	std::string convertToStringDEBUG();
};
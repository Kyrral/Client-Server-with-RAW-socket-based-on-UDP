#pragma once
#include <string>

struct Header {
	uint16_t sourcePort = 55555;
	uint16_t destinationPort = 0;
	uint16_t sequenceNumber = 0;
	uint16_t windowSize = 0;
	uint16_t dataOffset = sizeof(Header);
	bool SYN = false, FIN = false, RST = false;
	uint16_t ACK = 0;
	uint16_t checksum = 0;

	std::string convertToString();
	std::string convertToStringDEBUG();
	uint16_t computeChecksum();
};
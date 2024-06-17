#include "PacketManager.h"

Packet* PacketManager::prepareData(std::string message, int destinationPort, int sn) {
	Packet* responsePacket = new Packet(destinationPort, windowSize, sn);
	responsePacket->setPayload(message);
	return responsePacket;
}

void PacketManager::setWindowSize(int ws) { windowSize = ws; }

void PacketManager::setPackets(std::string& payload, const sockaddr_in& serverAddress) {
	int start = 0, fin = 10;
	while (start < payload.length()) {
		Packet* packet = prepareData(payload.substr(start, fin), serverAddress.sin_port, responseMessage->counter);

		if (start + fin >= payload.length()) {
			packet->header.FIN = true;
        }

		packet->header.checksum = packet->computeChecksum();

		responseMessage->packets.push_back(packet);
		start += 10;
		++responseMessage->counter;
	}
}

bool PacketManager::checkChecksum(Packet& packet) {
	uint16_t newChecksum = packet.computeChecksum();
	if (newChecksum == packet.header.checksum) return true;
	return false;
}
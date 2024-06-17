#include "PacketManager.h"

Packet* PacketManager::prepareData(std::string message, int sourcePort, int destinationPort, int sn) {
	Packet* responsePacket = new Packet(sourcePort, destinationPort, windowSize, sn);
	responsePacket->setPayload(message);
	return responsePacket;
}

//setters
int PacketManager::setMTU(int& socketfd) {
	ioctl(socketfd, SIOCGIFMTU, &MTU);
	return MTU.ifr_ifru.ifru_mtu;
}

void PacketManager::setWindowSize(int ws) { windowSize = ws; }

void PacketManager::setPackets(std::string& payload, const sockaddr_in& clientAddr, const sockaddr_in& serverAddress) {
	int start = 0, fin = 10;
	while (start < payload.length()) {
		Packet* packet = prepareData(payload.substr(start, fin), clientAddr.sin_port, serverAddress.sin_port, 0);

		if (start + fin >= payload.length()) {
			packet->header.FIN = true;
        }

		packet->header.checksum = packet->computeChecksum();

		responseMessage->packets.push_back(packet);
		start += 10;
	}
}

bool PacketManager::checkChecksum(Packet& packet) {
	uint16_t newChecksum = packet.computeChecksum();
	std::cout << "New checksum: " << newChecksum << std::endl;
	std::cout << "Old checksum: " << packet.header.checksum << std::endl;
	if (newChecksum == packet.header.checksum) return true;
	return false;
}
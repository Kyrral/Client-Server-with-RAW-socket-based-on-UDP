#include "Packet.h"

//constructors
Packet::Packet(uint16_t dp, uint16_t ws, uint16_t sn) {
    header.destinationPort = dp;
    header.windowSize = ws;
    header.sequenceNumber = sn;
    header.checksum = computeChecksum();
}

uint16_t Packet::computeChecksum() {
    uint32_t sum = header.computeChecksum();

    std::for_each(payload.begin(), payload.end(), [&sum](uint8_t byte) {
        sum += byte;
        if (sum > 0xFFFF)
            sum = (sum & 0xFFFF) + (sum >> 16);
    });

    return ~static_cast<uint16_t>(sum);
}

int Packet::BUFFER_SIZE = 0;

void Packet::setBufSize(int size) { BUFFER_SIZE = size; }
void Packet::setPayload(std::string payload) { this->payload = payload; }

void Packet::deserialize(char* buffer, int bytes_received) {
    std::vector<std::string> tokens;
    std::stringstream ss(std::string(buffer, bytes_received));
    std::string token;

    while (std::getline(ss, token, '\n')) {
        tokens.push_back(token);
    }

    int ul = std::stoi(tokens[1]);
    header.sourcePort = ul;

    ul = std::stoi(tokens[2]);
    header.destinationPort = static_cast<uint16_t>(ul);

    ul = std::stoi(tokens[3]);
    header.sequenceNumber = static_cast<uint16_t>(ul);

    ul = std::stoi(tokens[4]);
    header.windowSize = static_cast<uint16_t>(ul);

    ul = std::stoi(tokens[5]);
    header.checksum = static_cast<uint16_t>(ul);

    ul = std::stoi(tokens[6]);
    header.dataOffset = static_cast<uint16_t>(ul);

    ul = std::stoi(tokens[7]);
    header.SYN = static_cast<bool>(ul);

    ul = std::stoi(tokens[8]);
    header.FIN = static_cast<bool>(ul);

    ul = std::stoi(tokens[9]);
    header.RST = static_cast<bool>(ul);

    ul = std::stoi(tokens[10]);
    header.ACK = static_cast<uint16_t>(ul);

    payload = tokens[11];
}

int Packet::packetSize() { return 1024; }

std::string Packet::convertToString() {
    std::string str = header.convertToString();
    str += "\n" + payload + "\n";
    return str;
}

std::string Packet::convertToStringDEBUG() {
    std::string str = header.convertToStringDEBUG();
    str += "payload: ";
    str += payload;
    str += "\n";
    std::cout << str << std::endl;
    return str;
}

std::string Packet::getPayload() { return payload; }
#include "Header.h"

std::string Header::convertToString() {
    std::string str = "\n" + std::to_string(sourcePort);
    str += "\n" + std::to_string(destinationPort);
    str += "\n" + std::to_string(sequenceNumber);
    str += "\n" + std::to_string(windowSize);
    str += "\n" + std::to_string(checksum);
    str += "\n" + std::to_string(dataOffset);
    str += "\n" + std::to_string(SYN);
    str += "\n" + std::to_string(FIN);
    str += "\n" + std::to_string(RST);
    str += "\n" + std::to_string(ACK);

    return str;
}

std::string Header::convertToStringDEBUG() {
    std::string str = "sourcePort: " + std::to_string(sourcePort) + "\n";
    str += "destinationPort: " + std::to_string(destinationPort) + "\n";
    str += "sequenceNumber: " + std::to_string(sequenceNumber) + "\n";
    str += "windowSize: " + std::to_string(windowSize) + "\n";
    str += "checksum: " + std::to_string(checksum) + "\n";
    str += "dataOffset: " + std::to_string(dataOffset) + "\n";
    str += "SYN: " + std::to_string(SYN) + "\n";
    str += "FIN: " + std::to_string(FIN) + "\n";
    str += "RST: " + std::to_string(RST) + "\n";
    str += "ACK: " + std::to_string(ACK) + "\n";    

    return str;
}

uint16_t Header::computeChecksum() {
    const uint16_t* data = reinterpret_cast<const uint16_t*>(this);
    int length = sizeof(Header) / sizeof(uint16_t) - 1;

    uint32_t sum = 0;

    while (length > 0) {
        sum += *data++;
        length--;
    }

    while (sum >> 16) 
        sum = (sum & 0xFFFF) + (sum >> 16);

    return static_cast<uint16_t>(sum);
}
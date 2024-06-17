#include "Message.h"

void Message::constructMessage() {
    for (Packet* packet : packets) {
       message += packet->payload;
    }
}

std::string Message::getMessage() { return message; }
#include "Message.h"

void Message::constructMessage() {
    for (Packet* packet : packets) {
       message += packet->getPayload();
    }
}

std::string Message::getMessage() { return message; }
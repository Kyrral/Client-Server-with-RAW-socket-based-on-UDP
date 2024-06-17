#pragma once

#include <vector>
#include <string>
#include "Packet.h"

class Message {
private:
    std::string message = ""; 

public: 
    std::vector<Packet*> packets;
    int counter = 0;

    void constructMessage();

    //getters
    std::string getMessage();
};
#include "TCPClient.h"

int main() {
    TCPClient client;

    client.setConnection();
    client.receiveMessage();
    client.sendMessage();

    return 0;
}
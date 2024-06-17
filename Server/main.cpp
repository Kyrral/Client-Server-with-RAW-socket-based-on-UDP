#include "TCPServer.h"

int main() {
	TCPServer server;

	server.setConnection();
	server.sendMessage();
	server.receiveMessage();

	return 0;
}
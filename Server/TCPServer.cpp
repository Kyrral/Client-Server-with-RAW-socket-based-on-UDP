#include "TCPServer.h"

//socket
void TCPServer::setConnection() {
	if(createSocket()) {
		connect();

		Packet::setBufSize(pm->setMTU(socketfd));
		std::cout << "MTU: " << Packet::BUFFER_SIZE << std::endl;
		// requestWindowSize();
	}
}

bool TCPServer::createSocket() {
    if ((socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
		std::cerr << "socket creation failed" << std::endl;
		return false;
	}
    else {
        serverAddress.sin_family = AF_INET;
	    serverAddress.sin_port = htons(55555);
		serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

		if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr.s_addr) <= 0) {
        	std::cerr << "Error: Invalid address" << std::endl;
        	close(socketfd);
        	return false;
    	}

        if (bind(socketfd, (const sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
            std::cerr << "bind failed" << std::endl;
		std::cout << "socket created successfully" << std::endl;
		return true;	
    }
}

void TCPServer::connect() {
// receiving SYN
	Packet* syn_packet = receiveSYNACK();
	std::cout << "SYN received" << std::endl;

// prepare SYN-ACK packet
	Packet synack_packet;
	synack_packet.header.destinationPort = clientAddress.sin_port;
	synack_packet.header.sequenceNumber = rand.getRandNum();
	synack_packet.header.SYN = true;
	synack_packet.header.ACK = syn_packet->header.ACK + 1;
	synack_packet.payload = "SYN-ACK";
//
	auto start = std::chrono::steady_clock::now();

// sending SYN-ACK 
	if (sendSYNACK(synack_packet)) std::cout << "SYN-ACK sent" << std::endl;

// receiving ACK
	Packet* ack_packet = receiveSYNACK();
	if (ack_packet != nullptr) std::cout << "ACK received" << std::endl;

// setting RTT
	auto end = std::chrono::steady_clock::now();
	RTT = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    timeval timeout;
    timeout.tv_sec = RTT / 1000000;
    timeout.tv_usec = RTT % 1000000;

    if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        std::cerr << "Error: setsockopt failed\n";
        close(socketfd);
        return;
    }

	std::cout << "Connection established" << std::endl;
	
	//
	// std::cout << "Connection packets\n" << std::endl;
	// std::cout << "SYN packet (received):" << std::endl;
	// std::cout << syn_packet->convertToStringDEBUG() << std::endl;
	// std::cout << "\nSYN-ACK packet (sent):" << std::endl;
	// std::cout << synack_packet.convertToStringDEBUG() << std::endl;
	// std::cout << "\nACK packet (received):" << std::endl;
	// std::cout << ack_packet->convertToStringDEBUG() << std::endl;

	std::cout << "RTT: " << RTT << std::endl;
}

void TCPServer::requestWindowSize() {
	char temp_buff[Packet::BUFFER_SIZE];
	int bytes_received;
	
    socklen_t len = sizeof(clientAddress);

	sendto(socketfd, "Window size requested", strlen("Window size requested"), MSG_CONFIRM, (sockaddr*)&clientAddress, len);
	bytes_received = recvfrom(socketfd, temp_buff, sizeof(temp_buff), 0, (sockaddr*)&clientAddress, (socklen_t*)&len);

	pm->setWindowSize(atoi(temp_buff));
	std::cout << "window size " << atoi(temp_buff);
}

// packets
bool TCPServer::sendData(Packet& packet) {
    int bytes_sent = 0; 
    socklen_t len = sizeof(serverAddress);

    if (packet.header.ACK == 1) {
        bytes_sent = sendto(socketfd, (char*)packet.convertToString().c_str(), packet.packetSize(), 0, (sockaddr*)&serverAddress, len);
        return true;   
    }

	do {
    	bytes_sent = sendto(socketfd, (char*)packet.convertToString().c_str(), packet.packetSize(), 0, (sockaddr*)&serverAddress, len);
    	if (bytes_sent == -1) {
        	perror("sendto");
        	return false;
    	}
	} while (receiveData() == nullptr);
    
    return true;
}

bool TCPServer::sendSYNACK(Packet& packet) {
    int bytes_sent = 0; 
    socklen_t len = sizeof(serverAddress);

    bytes_sent = sendto(socketfd, (char*)packet.convertToString().c_str(), packet.packetSize(), 0, (sockaddr*)&serverAddress, len);
    if (bytes_sent == -1) {
        perror("sendto");
        return false;
    }
    
    return true;
}

Packet* TCPServer::receiveData() {
	char buffer[1024];
    int bytes_received = 0;
    socklen_t len = sizeof(clientAddress);

    bytes_received = recvfrom(socketfd, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&clientAddress, &len); 
    
    while (clientAddress.sin_port == serverAddress.sin_port) {
        bytes_received = recvfrom(socketfd, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&clientAddress, &len);
        if (bytes_received < 0) {
            std::cerr << "Error receiving packet" << std::endl;
        }
		else break;
    }

	Packet* packet = new Packet();
    packet->deserialize(buffer, bytes_received);

	if (packet->header.ACK != 1) {
		Packet resp_packet;
    	resp_packet.header.ACK = 1;
		sendData(resp_packet);
	}

	return packet;
}

Packet* TCPServer::receiveSYNACK() {
	char buffer[1024];
    int bytes_received = 0;
    socklen_t len = sizeof(serverAddress);

    bytes_received = recvfrom(socketfd, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&serverAddress, &len); 
    
    if (clientAddress.sin_port != serverAddress.sin_port) {
        bytes_received = recvfrom(socketfd, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&clientAddress, &len);
        if (bytes_received < 0) {
            std::cerr << "Error receiving packet" << std::endl;
            return nullptr;
        }
    }

    Packet* packet = new Packet();
    packet->deserialize(buffer, bytes_received);

    return packet;
}

bool TCPServer::sendMessage() {
	std::string message;
	std::cout << "Enter your message: ";
	std::cin >> message;

	pm->setPackets(message, clientAddress, serverAddress);
	for (Packet* packet : pm->responseMessage->packets) {
		sendData(*packet);
	}

	return true;
}

bool TCPServer::receiveMessage() {
	std::cout << "Waiting for message..." << std::endl;
    Packet* packet = nullptr;
    do {
        do packet = receiveData(); while (packet == nullptr);
        pm->requestMessage->packets.push_back(packet);
		pm->checkChecksum(*packet);
    } while (packet->header.FIN != true);

    std::cout << "Message received!" << std::endl;
    pm->requestMessage->constructMessage();
    std::cout << "Message: ";
    std::cout << pm->requestMessage->getMessage();

    return true;
}
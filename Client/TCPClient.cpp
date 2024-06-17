#include "TCPClient.h"

bool TCPClient::createSocket() {
    if ((socketfd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) < 0) {
		std::cerr << "socket creation failed" << std::endl;
		return false;
	}
    else {
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
            std::cerr << "Error: setsockopt failed\n";
            close(socketfd);
            return 1;
        }

        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(55555);
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

        expectedServerAddress.sin_family = serverAddress.sin_family;
        expectedServerAddress.sin_port = serverAddress.sin_port;
        expectedServerAddress.sin_addr.s_addr = serverAddress.sin_addr.s_addr;

        if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
            std::cerr << "Invalid address" << std::endl;
            return false;
        }
    }
    socklen_t addrLen = sizeof(clientAddr);
    getsockname(socketfd, (sockaddr*)&clientAddr, &addrLen);
	return true;
}

void TCPClient::connect() {
    std::string syn = "SYN", ack = "ACK";
    
    //prepare SYN packet
    Packet syn_packet;
    syn_packet.header.sequenceNumber = rand.getRandNum();
    syn_packet.header.SYN = true;

    syn_packet.setPayload(syn);
    //

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();;
    if (sendSYNACK(syn_packet) == true) std::cout << "SYN sent" << std::endl;
    Packet* synack_packet = receiveData();
    while (synack_packet == nullptr) {
        start = std::chrono::steady_clock::now();
        if (sendSYNACK(syn_packet) == true) std::cout << "SYN sent\n" << std::endl;
        else std::cout << "SYN is NOT sent" << std::endl;
        std::cout << "SYN-ACK is NOT received" << std::endl;
        synack_packet = receiveData();
    }
    std::cout << "SYN-ACK received\n" << std::endl;

    //set RTT
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
    //

    //prepare ACK packet
    Packet ack_packet;
    ack_packet.setPayload(ack);
    if (sendSYNACK(ack_packet) == true) std::cout << "ACK sent\n" << std::endl;
    else std::cout << "ACK is NOT sent" << std::endl;

    std::cout << "Connection established" << std::endl;
	std::cout << "SYN packet (sent):" << std::endl;
	std::cout << syn_packet.convertToStringDEBUG() << std::endl;
	std::cout << "\nSYN-ACK packet (received):" << std::endl;
	std::cout << synack_packet->convertToStringDEBUG() << std::endl;
	std::cout << "\nACK packet (sent):" << std::endl;
	std::cout << ack_packet.convertToStringDEBUG() << std::endl;

    std::cout << "RTT: " << RTT << std::endl;
}

void TCPClient::setConnection() {
    if (createSocket()) {
        connect();
        // requestWindowSize();
    }
}

void TCPClient::requestWindowSize() {
    sendMessage("Window size request");
	pm.setWindowSize(atoi(receiveData()->payload.c_str()));
}

Packet* TCPClient::receiveData() {
	char buffer[1024];
    int bytes_received = 0;
    socklen_t len = sizeof(serverAddress);

    bytes_received = recvfrom(socketfd, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&serverAddress, &len); 
    
    if (serverAddress.sin_port != expectedServerAddress.sin_port) {
        bytes_received = recvfrom(socketfd, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&serverAddress, &len);
        if (bytes_received < 0) {
            std::cerr << "Error receiving packet" << std::endl;
            return nullptr;
        }
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

Packet* TCPClient::receiveSYNACK() {
	char buffer[1024];
    int bytes_received = 0;
    socklen_t len = sizeof(serverAddress);

    bytes_received = recvfrom(socketfd, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&serverAddress, &len); 
    
    if (serverAddress.sin_port != expectedServerAddress.sin_port) {
        bytes_received = recvfrom(socketfd, (char*)buffer, sizeof(buffer), 0, (sockaddr*)&serverAddress, &len);
        if (bytes_received < 0) {
            std::cerr << "Error receiving packet" << std::endl;
            return nullptr;
        }
    }

    Packet* packet = new Packet();
    packet->deserialize(buffer, bytes_received);

    return packet;
}

bool TCPClient::sendData(Packet& packet) {
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

bool TCPClient::sendSYNACK(Packet& packet) {
    int bytes_sent = 0; 
    socklen_t len = sizeof(serverAddress);

    bytes_sent = sendto(socketfd, (char*)packet.convertToString().c_str(), packet.packetSize(), 0, (sockaddr*)&serverAddress, len);
    if (bytes_sent == -1) {
        perror("sendto");
        return false;
    }
    
    return true;
}

bool TCPClient::sendMessage() {
	std::string message;
	std::cout << "Enter your message: ";
	std::cin >> message;

	pm.setPackets(message, serverAddress);
	for (Packet* packet : pm.responseMessage->packets) {
		// std::cout << packet->convertToStringDEBUG();
		sendData(*packet);
	}

	return true;
}

bool TCPClient::sendMessage(std::string str) {
	pm.setPackets(str, serverAddress);
	for (Packet* packet : pm.responseMessage->packets)
		sendData(*packet);

	return true;
}

bool TCPClient::receiveMessage() {
	std::cout << "Waiting for message..." << std::endl;
    Packet* packet = nullptr;
    do {
        do packet = receiveData(); while (packet == nullptr);
        pm.checkChecksum(*packet);
        pm.requestMessage->packets.push_back(packet);
    } while (packet->header.FIN != true);

    std::cout << "Message received!" << std::endl;
    pm.requestMessage->constructMessage();
    std::cout << "Message: ";
    std::cout << pm.requestMessage->getMessage() << std::endl;

    return true;
}
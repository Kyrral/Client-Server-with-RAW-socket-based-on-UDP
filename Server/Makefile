CXX = g++

CXXFLAGS = -std=c++11 -Wall

SRCS = main.cpp Packet.cpp Message.cpp Header.cpp TCPServer.cpp PacketManager.cpp Randomizer.cpp

OBJS = $(SRCS:.cpp=.o)

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

start:
	sudo ./$(TARGET)
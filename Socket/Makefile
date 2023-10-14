CC = arm-linux-gnueabihf-g++
CFLAGS = -Wall -Wextra -std=c++11 -lrt -lpthread -static


all:
	$(CC) $(CFLAGS) UDPClient1-BBone.cpp -o network.sockket.udpclient1.out
	$(CC) $(CFLAGS) UDPClient2-Raspi.cpp -o network.sockket.udpclient2.out
	$(CC) $(CFLAGS) UDPServer.cpp -o network.socket.udpserver.out

clean:
	rm -f *.o *.out

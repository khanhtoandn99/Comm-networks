#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h> 
#include <stdlib.h> 

using namespace std;

#define SERVER_IP_ADDRESS    "192.168.0.3"
#define UDP_PORT          12340

#define PAYLOAD_MAX_SIZE 254

int server_fd = -1;
sockaddr_in m_serveraddr, m_clientaddr;

static void *runMsgReceiveLooper(void *arg)
{
    printf("[Server]  runMsgReceiveLooper()\n");
    (void)arg;
    static char rcvPacket[PAYLOAD_MAX_SIZE] = {0,};
    // memset(&m_clientaddr, 0x00, sizeof(m_clientaddr));
    while (true)
    {
        socklen_t temp = sizeof(m_clientaddr);
        if ((int)recvfrom(server_fd, (void *)&rcvPacket, PAYLOAD_MAX_SIZE, 0, (struct sockaddr *)&m_clientaddr, &temp) > 0)
        {
            printf("[Server]  Received a packet = \"%s\" from Client: %s\n", rcvPacket, inet_ntoa(m_clientaddr.sin_addr));
        }
    }
    printf("[Server]  runMsgReceiveLooper() Exited!\n");
    pthread_exit(nullptr);
}

int main()
{
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (server_fd < 0)
    {
        perror("ERROR opening socket");
        return -1;
    }

    memset(&m_serveraddr, 0x00, sizeof(sockaddr_in));
    m_serveraddr.sin_family = AF_INET;
    // m_serveraddr.sin_addr.s_addr = INADDR_ANY;
    m_serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
    m_serveraddr.sin_port = htons(UDP_PORT);

    if (bind(server_fd, (struct sockaddr*)&m_serveraddr, sizeof(m_serveraddr)) < 0)
    {
        perror("ERROR on binding");
        return -1;
    }

    // Run msg receive loop
    pthread_t m_pRcvThId;
    pthread_create(&m_pRcvThId, nullptr, runMsgReceiveLooper, nullptr);
    pthread_detach(m_pRcvThId);

    // Run msg send loop
    while (true)
    {
        char sendPacket[PAYLOAD_MAX_SIZE] = {0,};
        cin.getline (sendPacket, PAYLOAD_MAX_SIZE);
        printf("[Server]  Sending a packet= \"%s\" to Client: %s\n", sendPacket, inet_ntoa(m_clientaddr.sin_addr));
        if (sendto(server_fd, sendPacket, sizeof(sendPacket), 0 , (const struct sockaddr *)&m_clientaddr, sizeof(m_clientaddr)) == -1)
        {
            perror("[Server]  ERROR on sending");
        }
        cin.clear();
    }

    close(server_fd);
    return 0;
}
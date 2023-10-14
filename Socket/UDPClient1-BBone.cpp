#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

using namespace std;

#define SERVER_IP_ADDRESS    "192.168.0.3"
#define UDP_PORT          12340

#define PAYLOAD_MAX_SIZE 254

int server_fd = -1;
sockaddr_in m_serveraddr, m_otheraddr;

static void *runMsgReceiveLooper(void *arg)
{
    printf("[Client]  runMsgReceiveLooper()\n");
    (void)arg;
    static char rcvPacket[PAYLOAD_MAX_SIZE] = {0,};
    while (true)
    {
        socklen_t tmp_size;
        if ((int)recvfrom(server_fd, rcvPacket, PAYLOAD_MAX_SIZE, 0, (struct sockaddr*)&m_otheraddr, &tmp_size) > 0)
        {
            printf("[Client]  Received a packet = \"%s\" from %s\n", rcvPacket, inet_ntoa(m_otheraddr.sin_addr));
        }
    }

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


    // Client --> Don't need to bind

    // Run msg receive loop
    pthread_t m_pRcvThId;
    pthread_create(&m_pRcvThId, nullptr, runMsgReceiveLooper, nullptr);
    pthread_detach(m_pRcvThId);

    // Run msg send loop
    while (true)
    {
        char sendPacket[PAYLOAD_MAX_SIZE] = {0,};
        cin.getline (sendPacket, PAYLOAD_MAX_SIZE);
        printf("[Client]  Sending a packet: \"%s\" to Server: %s\n", sendPacket, inet_ntoa(m_serveraddr.sin_addr));
        if (sendto(server_fd, sendPacket, sizeof(sendPacket), 0 , (const struct sockaddr *)&m_serveraddr, sizeof(m_serveraddr)) == -1)
        {
            perror("[Client]  ERROR on sending");
        }
        cin.clear();
    }

    close(server_fd);
}
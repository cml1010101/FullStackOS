#ifndef TCP_H
#define TCP_H
#include <SmartOS.h>
#ifdef __cplusplus
#define TCP_SYN 0x2
#define TCP_FIN 0x1
#define TCP_PSH 0x8
#define TCP_ACK 0x10
struct __attribute__((packed)) TCPPseudoHeader
{
    uint8_t srcIP[4];
    uint8_t destIP[4];
    uint8_t rsv;
    uint8_t protocol;
    uint16_t tcpLength;
};
struct __attribute__((packed)) TCPHeader
{
    uint16_t sourcePort;
    uint16_t destPort;
    uint32_t sequenceNumber;
    uint32_t ackNumber;
    uint16_t flags;
    uint16_t windowSize;
    uint16_t checksum;
    uint16_t urgPtr;
};
enum struct TCPState
{
    CLOSED,
    LISTEN,
    SYN_SENT,
    SYN_RECIEVED,
    ESTABLISHED,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSE_WAIT,
    CLOSING,
    LAST_ACK,
    TIME_WAIT
};
struct TCPConnection
{
    TCPState state;
    uint16_t localPort;
    uint16_t remotePort;
    uint8_t destIP[4];
    uint32_t sequenceNumber;
    uint32_t ackNumber;
    TCPConnection() = default;
    inline TCPConnection(uint16_t localPort, uint16_t destPort, uint8_t destIP[4])
    {
        this->localPort = localPort;
        this->remotePort = destPort;
        memcpy(this->destIP, destIP, 4);
    }
};
struct TCPHandler
{
    uint16_t portNo;
    void(*handler)(TCPConnection* conn, const void* data, size_t len, EthernetDevice* dev);
};
extern Vector<TCPHandler> tcpHandlers;
TCPConnection* newTCP(uint16_t localPort, uint8_t* destIP, uint16_t destPort);
void openTCP(TCPConnection* conn, EthernetDevice* dev);
void closeTCP(TCPConnection* conn, EthernetDevice* dev);
void tcpRecieve(TCPHeader* header, uint8_t senderIP[4], size_t totalSize, EthernetDevice* dev);
void tcpSendData(TCPConnection* conn, const void* data, size_t size, EthernetDevice* dev);
void initializeTCP();
#endif
#endif
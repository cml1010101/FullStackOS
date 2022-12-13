#ifndef TCP_H
#define TCP_H
#include <SmartOS.h>
#ifdef __cplusplus
struct __attribute__((packed)) TCPHeader
{
    uint16_t sourcePort;
    uint16_t destPort;
    uint32_t sequenceNumber;
    uint32_t ackNumber;
    uint16_t dataOffset: 4;
    uint16_t rsv: 3;
    uint16_t ns: 1;
    uint16_t cwr: 1;
    uint16_t ece: 1;
    uint16_t urg: 1;
    uint16_t ack: 1;
    uint16_t psh: 1;
    uint16_t rst: 1;
    uint16_t syn: 1;
    uint16_t fin: 1;
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
    void (*dataHandler)(void* data, size_t size);
    TCPConnection() = default;
    inline TCPConnection(uint16_t localPort, uint16_t destPort, uint8_t destIP[4])
    {
        this->localPort = localPort;
        this->remotePort = destPort;
        memcpy(this->destIP, destIP, 4);
    }
};
TCPConnection* newTCP(uint16_t localPort, uint8_t* destIP, uint16_t destPort);
void openTCP(TCPConnection* conn, EthernetDevice* dev);
void closeTCP(TCPConnection* conn, EthernetDevice* dev);
void tcpRecieve(TCPHeader* header, uint8_t senderIP[4], EthernetDevice* dev);
void tcpSendData(TCPConnection* conn, const void* data, size_t size, EthernetDevice* dev);
void initializeTCP();
#endif
#endif
#include <TCP.h>
#include <IP.h>
#include <Ethernet.h>
#define OPT_END 0
#define OPT_NOP 1
#define OPT_MSS 2
Vector<TCPHandler> tcpHandlers;
uint16_t tcp_calculate_checksum(TCPHeader* packet, TCPConnection* conn, size_t dataLength)
{
    size_t totalLength = dataLength + sizeof(TCPHeader);
    void* tmp = malloc(((sizeof(TCPHeader) + sizeof(TCPPseudoHeader) + dataLength + 1) / 2) * 2);
    if (dataLength & 1)
    {
        ((uint8_t*)tmp)[sizeof(TCPHeader) + sizeof(TCPPseudoHeader) + dataLength] = 0;
    }
    TCPPseudoHeader* phdr = (TCPPseudoHeader*)tmp;
    phdr->protocol = PROTOCOL_TCP;
    phdr->rsv = 0;
    phdr->tcpLength = ntohs(totalLength);
    memcpy(phdr->destIP, conn->destIP, 4);
    memcpy(phdr->srcIP, getSourceIP(), 4);
    memcpy(tmp + sizeof(TCPPseudoHeader), packet, sizeof(TCPHeader));
    memcpy(tmp + sizeof(TCPPseudoHeader) + sizeof(TCPHeader), &packet[1], dataLength);
    int arraySize = (sizeof(TCPHeader) + sizeof(TCPPseudoHeader) + dataLength + 1) / 2;
    uint16_t* array = (uint16_t*)tmp;
    uint32_t sum = 0;
    for (int i = 0; i < arraySize; i++)
    {
        sum += ntohs(array[i]);
    }
    uint32_t carry = sum >> 16;
    sum = sum & 0x0000ffff;
    sum = sum + carry;
    uint16_t ret = ~sum;
    return ret;
}
Vector<TCPConnection*> tcpConnections;
void tcpSendSyn(TCPConnection* conn, EthernetDevice* dev);
TCPConnection* newTCP(uint16_t localPort, uint8_t* destIP, uint16_t destPort)
{
    TCPConnection* conn = new TCPConnection;
    conn->localPort = localPort;
    conn->remotePort = destPort;
    conn->ackNumber = 0;
    conn->sequenceNumber = 0;
    conn->state = TCPState::CLOSED;
    memcpy(conn->destIP, destIP, 4);
    tcpConnections.push(conn);
    return conn;
}
TCPConnection* getTCP(TCPHeader* header, uint8_t senderIP[4])
{
    for (size_t i = 0; i < tcpConnections.size(); i++)
        if (tcpConnections[i]->localPort == ntohs(header->destPort)
            && tcpConnections[i]->remotePort == ntohs(header->sourcePort)) return tcpConnections[i];
    return newTCP(ntohs(header->destPort), senderIP, ntohs(header->sourcePort));
}
void tcpSend(TCPConnection* conn, EthernetDevice* dev, uint16_t flags, void* data, size_t len)
{
    TCPHeader* header = (TCPHeader*)malloc(sizeof(TCPHeader) + (flags & TCP_SYN ? 4 : 0) + len);
    memset(header, 0, sizeof(TCPHeader));
    header->flags = flags;
    header->ackNumber = htonl(conn->ackNumber);
    header->sequenceNumber = htonl(conn->sequenceNumber);
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->flags |= ((sizeof(TCPHeader) + (flags & TCP_SYN ? 4 : 0)) / 4) << 12;
    header->flags = ntohs(header->flags);
    header->windowSize = 0xFFFF;
    uint8_t* p = (uint8_t*)(&header[1]);
    if (flags & TCP_SYN)
    {
        p[0] = OPT_MSS;
        p[1] = 4;
        *(uint16_t*)(&p[2]) = ntohs(1460);
        p += 4;
    }
    memcpy(p, data, len);
    header->checksum = ntohs(tcp_calculate_checksum(header, conn,
        (flags & TCP_SYN ? 4 : 0) + len));
    qemu_printf("Sending some data over TCP\n");
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader) + (flags & TCP_SYN ? 4 : 0) + len,
        PROTOCOL_TCP, dev);
}
void tcpSendSyn(TCPConnection* conn, EthernetDevice* dev)
{
    tcpSend(conn, dev, TCP_SYN, NULL, 0);
}
void openTCP(TCPConnection* conn, EthernetDevice* dev)
{
    conn->state = TCPState::SYN_SENT;
    tcpSendSyn(conn, dev);
}
void tcpSendSynAck(TCPConnection* conn, EthernetDevice* dev)
{
    tcpSend(conn, dev, TCP_SYN | TCP_ACK, NULL, 0);
}
void tcpSendFinAck(TCPConnection* conn, EthernetDevice* dev)
{
    tcpSend(conn, dev, TCP_FIN | TCP_ACK, NULL, 0);
}
void tcpSendAck(TCPConnection* conn, EthernetDevice* dev)
{
    tcpSend(conn, dev, TCP_ACK, NULL, 0);
}
void closeTCP(TCPConnection* conn, EthernetDevice* dev)
{
    conn->state = TCPState::CLOSING;
    tcpSendFinAck(conn, dev);
}
void tcpRecieve(TCPHeader* packet, uint8_t ip4[4], size_t totalSize, EthernetDevice* dev)
{
    size_t packetLen = totalSize - sizeof(TCPHeader);
    TCPConnection* conn = getTCP(packet, ip4);
    conn->sequenceNumber = htonl(packet->ackNumber);
    conn->ackNumber = htonl(packet->sequenceNumber) + ((packetLen == 0 ||
        ntohs(packet->flags) & TCP_SYN) ? 1 : packetLen);
    if ((ntohs(packet->flags) & (TCP_ACK | TCP_SYN)) == (TCP_ACK | TCP_SYN))
    {
        conn->state = TCPState::ESTABLISHED;
        tcpSendAck(conn, dev);
    }
    else if ((ntohs(packet->flags) & (TCP_ACK | TCP_PSH)) == (TCP_ACK | TCP_PSH))
    {
        for (size_t i = 0; i < tcpHandlers.size(); i++)
            if (tcpHandlers[i].portNo == ntohs(packet->destPort))
                tcpHandlers[i].handler(conn, (uint8_t*)packet + ((ntohs(packet->flags) & 0xF000) >> 10), 
                    packetLen - (packet->flags & TCP_SYN ? 4 : 0), dev);
        tcpSendAck(conn, dev);
    }
    else if ((ntohs(packet->flags) & (TCP_ACK | TCP_FIN)) == (TCP_ACK | TCP_FIN))
    {
        if (conn->state == TCPState::ESTABLISHED)
            tcpSendFinAck(conn, dev);
        else
            tcpSendAck(conn, dev);
        conn->state = TCPState::CLOSED;
    }
    else if (ntohs(packet->flags) & (TCP_SYN))
    {
        conn->state = TCPState::ESTABLISHED;
        tcpSendSynAck(conn, dev);
    }
}
void tcpSendData(TCPConnection* conn, const void* data, size_t len, EthernetDevice* dev)
{
    tcpSend(conn, dev, TCP_PSH | TCP_ACK, (void*)data, len);
}
void initializeTCP()
{
    tcpConnections = {};
    tcpHandlers = {};
}
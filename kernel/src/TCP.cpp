#include <TCP.h>
#include <IP.h>
#include <Ethernet.h>
Vector<TCPHandler> tcpHandlers;
uint16_t tcp_calculate_checksum(TCPHeader* packet, TCPConnection* conn, size_t dataLength)
{
    size_t totalLength = dataLength + sizeof(TCPHeader);
    void* tmp = malloc(sizeof(TCPHandler) + sizeof(TCPPseudoHeader));
    TCPPseudoHeader* phdr = (TCPPseudoHeader*)tmp;
    phdr->protocol = PROTOCOL_TCP;
    phdr->rsv = 0;
    phdr->tcpLength = ntohs(totalLength);
    memcpy(phdr->destIP, conn->destIP, 4);
    memcpy(phdr->srcIP, getSourceIP(), 4);
    memcpy(tmp + sizeof(TCPPseudoHeader), packet, sizeof(TCPHeader));
    int arraySize = (sizeof(TCPHeader) + sizeof(TCPPseudoHeader)) / 2;
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
        if (tcpConnections[i]->localPort == ntohs(header->destPort)) return tcpConnections[i];
    return newTCP(ntohs(header->destPort), senderIP, ntohs(header->sourcePort));
}
void tcpSendSyn(TCPConnection* conn, EthernetDevice* dev)
{
    TCPHeader* header = new TCPHeader;
    memset(header, 0, sizeof(TCPHeader));
    header->flags = TCP_SYN;
    header->ackNumber = htonl(conn->ackNumber);
    header->sequenceNumber = htonl(conn->sequenceNumber);
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->flags |= (sizeof(TCPHeader) / 4) << 12;
    header->flags = ntohs(header->flags);
    header->windowSize = 0xFFFF;
    header->checksum = ntohs(tcp_calculate_checksum(header, conn, 0));
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader), PROTOCOL_TCP, dev);
}
void openTCP(TCPConnection* conn, EthernetDevice* dev)
{
    conn->state = TCPState::SYN_SENT;
    tcpSendSyn(conn, dev);
}
void tcpSendSynAck(TCPConnection* conn, EthernetDevice* dev)
{
    TCPHeader* header = new TCPHeader;
    memset(header, 0, sizeof(TCPHeader));
    header->flags = TCP_SYN | TCP_ACK;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->flags |= (sizeof(TCPHeader) / 4) << 12;
    header->flags = ntohs(header->flags);
    header->windowSize = 0xFFFF;
    header->checksum = ntohs(tcp_calculate_checksum(header, conn, 0));
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader), PROTOCOL_TCP, dev);
}
void tcpSendFinAck(TCPConnection* conn, EthernetDevice* dev)
{
    TCPHeader* header = new TCPHeader;
    memset(header, 0, sizeof(TCPHeader));
    header->flags = TCP_FIN | TCP_ACK;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->flags |= (sizeof(TCPHeader) / 4) << 12;
    header->flags = ntohs(header->flags);
    header->windowSize = 0xFFFF;
    header->checksum = ntohs(tcp_calculate_checksum(header, conn, 0));
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader), PROTOCOL_TCP, dev);
}
void tcpSendAck(TCPConnection* conn, EthernetDevice* dev)
{
    TCPHeader* header = new TCPHeader;
    memset(header, 0, sizeof(TCPHeader));
    header->flags = TCP_ACK;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->flags |= (sizeof(TCPHeader) / 4) << 12;
    header->flags = ntohs(header->flags);
    header->windowSize = 0xFFFF;
    header->checksum = ntohs(tcp_calculate_checksum(header, conn, 0));
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader), PROTOCOL_TCP, dev);
}
void closeTCP(TCPConnection* conn, EthernetDevice* dev)
{
    conn->state = TCPState::CLOSING;
    tcpSendFinAck(conn, dev);
}
void tcpRecieve(TCPHeader* packet, uint8_t ip4[4], size_t totalSize, EthernetDevice* dev)
{
    qemu_printf("TCP recieved packet\n");
    size_t packetLen = totalSize - sizeof(TCPHeader);
    TCPConnection* conn = getTCP(packet, ip4);
    conn->sequenceNumber = htonl(packet->ackNumber);
    conn->ackNumber = htonl(packet->sequenceNumber) + ((packetLen == 0) ? 1 : packetLen);
    qemu_printf("%d to %d\n", conn->remotePort, conn->localPort);
    qemu_printf("%x\n", ntohs(packet->flags));
    if ((ntohs(packet->flags) & (TCP_ACK | TCP_SYN)) == (TCP_ACK | TCP_SYN))
    {
        qemu_printf("ACK/SYN\n");
        conn->state = TCPState::ESTABLISHED;
        tcpSendAck(conn, dev);
    }
    else if ((ntohs(packet->flags) & (TCP_ACK | TCP_PSH)) == (TCP_ACK | TCP_PSH))
    {
        qemu_printf("ACK/PSH\n");
        for (size_t i = 0; i < tcpHandlers.size(); i++)
            if (tcpHandlers[i].portNo == ntohs(packet->destPort))
                tcpHandlers[i].handler(conn, (uint8_t*)packet + ((packet->flags & 0xF000) >> 10), 
                    packetLen, dev);
        tcpSendAck(conn, dev);
    }
    else if ((ntohs(packet->flags) & (TCP_ACK | TCP_FIN)) == (TCP_ACK | TCP_FIN))
    {
        qemu_printf("ACK/FIN\n");
        if (conn->state == TCPState::ESTABLISHED)
            tcpSendFinAck(conn, dev);
        else
            tcpSendAck(conn, dev);
        conn->state = TCPState::CLOSED;
    }
    else if (ntohs(packet->flags) & (TCP_SYN))
    {
        qemu_printf("SYN\n");
        conn->state = TCPState::ESTABLISHED;
        tcpSendSynAck(conn, dev);
    }
}
void tcpSendData(TCPConnection* conn, const void* data, size_t len, EthernetDevice* dev)
{
    TCPHeader* header = (TCPHeader*)malloc(sizeof(TCPHeader) + len);
    memset(header, 0, sizeof(TCPHeader));
    header->flags = TCP_PSH | TCP_ACK;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->flags |= (sizeof(TCPHeader) / 4) << 12;
    header->flags = ntohs(header->flags);
    header->windowSize = 0xFFFF;
    header->checksum = ntohs(tcp_calculate_checksum(header, conn, len));
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader) + len, PROTOCOL_TCP, dev);
}
void initializeTCP()
{
    tcpConnections = {};
    tcpHandlers = {};
}
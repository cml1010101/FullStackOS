#include <TCP.h>
#include <IP.h>
uint16_t tcp_calculate_checksum(TCPHeader* packet) {
    int arraySize = sizeof(TCPHeader) / 2;
    uint16_t* array = (uint16_t*)packet;
    uint8_t* array2 = (uint8_t*)packet;
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
    header->syn = 1;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->dataOffset = sizeof(TCPHeader);
    header->checksum = tcp_calculate_checksum(header);
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
    header->syn = 1;
    header->ack = 1;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->dataOffset = sizeof(TCPHeader);
    header->checksum = tcp_calculate_checksum(header);
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader), PROTOCOL_TCP, dev);
}
void tcpSendFinAck(TCPConnection* conn, EthernetDevice* dev)
{
    TCPHeader* header = new TCPHeader;
    memset(header, 0, sizeof(TCPHeader));
    header->fin = 1;
    header->ack = 1;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->dataOffset = sizeof(TCPHeader);
    header->checksum = tcp_calculate_checksum(header);
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader), PROTOCOL_TCP, dev);
}
void tcpSendAck(TCPConnection* conn, EthernetDevice* dev)
{
    TCPHeader* header = new TCPHeader;
    memset(header, 0, sizeof(TCPHeader));
    header->ack = 1;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->dataOffset = sizeof(TCPHeader);
    header->checksum = tcp_calculate_checksum(header);
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader), PROTOCOL_TCP, dev);
}
void closeTCP(TCPConnection* conn, EthernetDevice* dev)
{
    conn->state = TCPState::CLOSING;
    tcpSendFinAck(conn, dev);
}
void tcpRecieve(TCPHeader* packet, uint8_t ip4[4], EthernetDevice* dev)
{
    TCPConnection* conn = getTCP(packet, ip4);
    size_t packetLen = ntohs(packet->windowSize);
    conn->sequenceNumber = packet->ackNumber;
    conn->ackNumber = packet->sequenceNumber + ((packetLen == 0) ? 1 : packetLen);
    if (packet->ack && packet->syn)
    {
        conn->state = TCPState::ESTABLISHED;
        tcpSendAck(conn, dev);
    }
    else if (packet->ack && packet->psh)
    {
        conn->dataHandler((uint8_t*)packet + packet->dataOffset, packetLen);
        tcpSendAck(conn, dev);
    }
    else if (packet->syn && packet->fin)
    {
        if (conn->state == TCPState::ESTABLISHED)
            tcpSendFinAck(conn, dev);
        else
            tcpSendAck(conn, dev);
        conn->state = TCPState::CLOSED;
    }
    else if (packet->syn)
    {
        conn->state = TCPState::ESTABLISHED;
        tcpSendSynAck(conn, dev);
    }
}
void tcpSendData(TCPConnection* conn, const void* data, size_t len, EthernetDevice* dev)
{
    TCPHeader* header = (TCPHeader*)malloc(sizeof(TCPHeader) + len);
    memset(header, 0, sizeof(TCPHeader));
    header->psh = 1;
    header->ack = 1;
    header->destPort = ntohs(conn->remotePort);
    header->sourcePort = ntohs(conn->localPort);
    header->dataOffset = sizeof(TCPHeader);
    header->windowSize = len;
    header->checksum = tcp_calculate_checksum(header);
    ipSendPacket(conn->destIP, header, sizeof(TCPHeader) + len, PROTOCOL_TCP, dev);
}
void initializeTCP()
{
    tcpConnections = {};
}
#include <HTTP.h>
char* htmlData;
size_t htmlSize;
void httpHandler(TCPConnection* conn, const void* data, size_t len, EthernetDevice* dev)
{
    qemu_printf("Got data\n");
}
void initializeHTMLFrontend()
{
    File* file = fileSystems[0]->open("/RES/INDEX.HTML");
    htmlSize = file->getSize();
    htmlData = new char[htmlSize];
    file->read(htmlData, htmlSize);
    TCPHandler handler;
    handler.portNo = 8080;
    handler.handler = httpHandler;
    tcpHandlers.push(handler);
}
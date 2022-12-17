#include <HTTP.h>
char* htmlData;
size_t htmlSize, notFoundSize, styleSize;
char* nfData, *style;
HTTPRequest parseHTTPRequest(const void* dat, size_t len)
{
    qemu_printf("Parsing HTTP Request\n");
    const char* data = (const char*)dat;
    HTTPRequest request;
    size_t j = 0, i;
    const char* option;
    for (i = 0; i < len; i++)
    {
        if (data[i] == '\n')
        {
            option = data + j;
            size_t optionLength = i - j - 1;
            j = i + 1;
            if (optionLength == 0)
            {
                break;
            }
            char* tmp = new char[optionLength + 1];
            memcpy(tmp, option, optionLength);
            tmp[optionLength] = 0;
            if (memcmp(option, "GET", 3) == 0)
            {
                request.type = RequestType::GET;
                size_t k = 0;
                while (option[k + 4] != ' ') k++;
                char* loc = new char[k + 1];
                loc[k] = 0;
                memcpy(loc, option + 4, k);
                request.requestLocation = loc;
            }
            else if (memcmp(option, "POST", 4) == 0)
            {
                request.type == RequestType::POST;
            }
        }
    }
    char* ndat = new char[len - i - 1];
    memcpy(ndat, option + 2, len - i - 1);
    request.dataLength = len - i - 1;
    request.data = ndat;
    return request;
}
const char* makeHTTPPacket(Vector<const char*> packetOptions, const char* content)
{
    const char* packet = "";
    while (packetOptions.size() != 0)
    {
        packet = strcat(packet, strcat(packetOptions.popTop(), "\r\n"));
    }
    packet = strcat(packet, "\r\n");
    packet = strcat(packet, content);
    return packet;
}
void httpHandler(TCPConnection* conn, const void* data, size_t len, EthernetDevice* dev)
{
    qemu_printf("Recieved TCP Data\n");
    HTTPRequest request = parseHTTPRequest(data, len);
    if (request.type == RequestType::GET)
    {
        if (strcmp(request.requestLocation, "/") == 0)
        {
            Vector<const char*> options = {};
            options.push("HTTP/1.1 200 OK");
            options.push("Date: Mon, 27 Jul 2009 12:28:53 GMT");
            options.push("Server: Apache/2.2.14 (Win32)");
            options.push("Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT");
            char* str = new char[50];
            memcpy(str, itoa(htmlSize, 10), 50);
            options.push(strcat("Content-Length: ", str));
            options.push("Content-Type: text/html");
            options.push("Connection: Closed");
            const char* packet = makeHTTPPacket(options, htmlData);
            qemu_printf("Sending %s\n", packet);
            tcpSendData(conn, packet, strlen(packet), dev);
        }
        else if (strcmp(request.requestLocation, "/style.css") == 0)
        {
            Vector<const char*> options = {};
            options.push("HTTP/1.1 200 OK");
            options.push("Date: Mon, 27 Jul 2009 12:28:53 GMT");
            options.push("Server: Apache/2.2.14 (Win32)");
            options.push("Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT");
            char* str = new char[50];
            memcpy(str, itoa(styleSize, 10), 50);
            options.push(strcat("Content-Length: ", str));
            options.push("Content-Type: text/css");
            options.push("Connection: Closed");
            const char* packet = makeHTTPPacket(options, style);
            qemu_printf("Sending %s\n", packet);
            tcpSendData(conn, packet, strlen(packet), dev);
        }
        else
        {
            Vector<const char*> options = {};
            options.push("HTTP/1.1 404 Not Found");
            options.push("Date: Mon, 27 Jul 2009 12:28:53 GMT");
            options.push("Server: Apache/2.2.14 (Win32)");
            options.push("Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT");
            char* str = new char[50];
            memcpy(str, itoa(notFoundSize, 10), 50);
            options.push(strcat("Content-Length: ", str));
            options.push("Content-Type: text/html");
            options.push("Connection: Closed");
            const char* packet = makeHTTPPacket(options, nfData);
            qemu_printf("Sending %s\n", packet);
            tcpSendData(conn, packet, strlen(packet), dev);
        }
    }
}
void initializeHTMLFrontend()
{
    File* file = fileSystems[0]->open("/RES/INDEX.HTM");
    htmlSize = file->getSize();
    htmlData = new char[htmlSize + 1];
    file->read(htmlData, htmlSize);
    htmlData[htmlSize] = 0;
    file = fileSystems[0]->open("/RES/ERROR.HTM");
    notFoundSize = file->getSize();
    nfData = new char[notFoundSize + 1];
    file->read(nfData, notFoundSize);
    nfData[notFoundSize] = 0;
    file = fileSystems[0]->open("/RES/STYLE.CSS");
    styleSize = file->getSize();
    style = new char[styleSize + 1];
    file->read(style, styleSize);
    style[styleSize] = 0;
    TCPHandler handler;
    handler.portNo = 8080;
    handler.handler = httpHandler;
    tcpHandlers.push(handler);
}
#include <HTTP.h>
#include <Shell.h>
#include <JSON.h>
char* htmlData;
size_t htmlSize, notFoundSize, styleSize, scriptSize, consoleSize;
char* nfData, *style;
char* script, *console;
HTTPRequest parseHTTPRequest(const void* dat, size_t len)
{
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
                request.type = RequestType::POST;
                size_t k = 0;
                while (option[k + 4] != ' ') k++;
                char* loc = new char[k + 1];
                loc[k] = 0;
                memcpy(loc, option + 4, k);
                request.requestLocation = loc;
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
    HTTPRequest request = parseHTTPRequest(data, len);
    qemu_printf("Recieved: %s\n", data);
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
            tcpSendData(conn, packet, strlen(packet), dev);
            free(str);
        }
        else if (fileSystems[0]->exists(strcat("/res", request.requestLocation)))
        {
            Vector<const char*> options = {};
            options.push("HTTP/1.1 200 OK");
            options.push("Date: Mon, 27 Jul 2009 12:28:53 GMT");
            options.push("Server: Apache/2.2.14 (Win32)");
            options.push("Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT");
            const char* type = "html";
            if (request.requestLocation[strlen(request.requestLocation) - 1] == 's'
                && request.requestLocation[strlen(request.requestLocation) - 2] == 's'
                && request.requestLocation[strlen(request.requestLocation) - 3] == 'c')
            {
                type = "css";
            }
            if (request.requestLocation[strlen(request.requestLocation) - 1] == 's'
                && request.requestLocation[strlen(request.requestLocation) - 2] == 'j')
            {
                type = "js";
            }
            options.push(strcat("Content-Type: text/", type));
            options.push("Connection: Closed");
            File* file = fileSystems[0]->open(strcat("/res", request.requestLocation));
            scriptSize = file->getSize();
            script = new char[scriptSize + 1];
            file->read(script, scriptSize);
            script[scriptSize] = 0;
            char* str = new char[50];
            memcpy(str, itoa(scriptSize, 10), 50);
            options.push(strcat("Content-Length: ", str));
            const char* packet = makeHTTPPacket(options, script);
            tcpSendData(conn, packet, strlen(packet), dev);
            free(str);
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
            tcpSendData(conn, packet, strlen(packet), dev);
            free(str);
        }
    }
    else if (request.type == RequestType::POST)
    {
        size_t j = 0, k = 0, c = 0;
        char* tmp = new char[request.dataLength + 1];
        tmp[request.dataLength] = 0;
        memcpy(tmp, request.data, request.dataLength);
        JSONNode node = parseJSON(tmp);
        const char* command = node.getProperty<const char*>("command");
        Vector<const char*> options = {};
        const char* resp = handleShell(command);
        if (resp == NULL) return;
        JSONNode responseNode;
        responseNode.setProperty("response", resp);
        const char* response = responseNode.toString();
        options.push("HTTP/1.1 200 OK");
        options.push("Date: Mon, 27 Jul 2009 12:28:53 GMT");
        options.push("Server: Apache/2.2.14 (Win32)");
        options.push("Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT");
        char* str = new char[50];
        memcpy(str, itoa(strlen(response), 10), 50);
        options.push(strcat("Content-Length: ", str));
        options.push("Content-Type: application/json");
        options.push("Connection: Closed");
        const char* packet = makeHTTPPacket(options, response);
        tcpSendData(conn, packet, strlen(packet), dev);
        free(str);
    }
}
void initializeHTMLFrontend()
{
    File* file = fileSystems[0]->open("/res/index.htm");
    htmlSize = file->getSize();
    htmlData = new char[htmlSize + 1];
    file->read(htmlData, htmlSize);
    htmlData[htmlSize] = 0;
    file = fileSystems[0]->open("/res/error.htm");
    notFoundSize = file->getSize();
    nfData = new char[notFoundSize + 1];
    file->read(nfData, notFoundSize);
    nfData[notFoundSize] = 0;
    TCPHandler handler;
    handler.portNo = 8080;
    handler.handler = httpHandler;
    tcpHandlers.push(handler);
}
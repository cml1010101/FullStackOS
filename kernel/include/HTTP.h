#ifndef HTTP_H
#define HTTP_H
#include <TCP.h>
#ifdef __cplusplus
enum struct RequestType
{
    GET,
    POST
};
struct HTTPRequest
{
    RequestType type;
    const char* requestLocation;
    const char* data;
    size_t dataLength;
};
void initializeHTMLFrontend();
#endif
#endif
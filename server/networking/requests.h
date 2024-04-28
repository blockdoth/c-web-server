#ifndef WEBSITE_REQUESTS_H
#define WEBSITE_REQUESTS_H
#include <errno.h>
#include "malloc.h"
#include "string.h"


typedef enum {
    GET,
    POST,
    PUT,
    DELETE
} REQUEST_TYPE;

typedef struct {
    char* url;
    int depth;
} URL;


typedef enum {
    HTML,
    CSS,
    TXT,
    MD,
    ICO,
    NVT,
} Extension;


typedef struct {
    REQUEST_TYPE requestType;
    URL url;
    Extension extension;
    char* body;
} Request;




//char* getExtension(const char* path);
//Extension parseExtension(char* url);
//URL parseURL(char* buffer);
//char* parseBody(const char* buffer);
//char* getURLChunk(URL url, int n);
//REQUEST_TYPE parseRequestType(char* buffer);
Request parseRequest(char* buffer);
char* match(const char* buffer, const char* pattern);
void destroyRequest(Request request);
void printRequest(Request request);
void destroyRequest(Request request);
#endif //WEBSITE_REQUESTS_H

#ifndef WEBSITE_ENDPOINTS_H
#define WEBSITE_ENDPOINTS_H

#include "requests.h"

#define MAX_ENDPOINTS 10

typedef struct {
    char* path;
    REQUEST_TYPE type;
    char*(*endpoint)(Request request);
} Endpoint;


void addEndpoint(REQUEST_TYPE type, char* path, char*(*endpoint)(Request request));
char* callEndpoint(Request request);
void destroyEndpoints();

#endif //WEBSITE_ENDPOINTS_H

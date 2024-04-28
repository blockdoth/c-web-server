#ifndef WEBSITE_ENDPOINTS_H
#define WEBSITE_ENDPOINTS_H

#include "requests.h"

#define MAX_ENDPOINTS 10

typedef struct {
    char* path;
    REQUEST_TYPE type;
    void(*endpoint)(Request request);
} Endpoint;

Endpoint* endpoints[MAX_ENDPOINTS];
int endpointPointer = 0;

void addEndpoint(REQUEST_TYPE type, char* path, void(*endpoint)(Request request));

void destroyEndpoints();

#endif //WEBSITE_ENDPOINTS_H

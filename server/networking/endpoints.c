#include "endpoints.h"


void destroyEndpoints(){
    while (endpointPointer > 0){
        free(endpoints[endpointPointer--]);
    }
}

void addEndpoint(REQUEST_TYPE type, char* path, void(*endpoint)(Request request)){
    if(endpointPointer >= MAX_ENDPOINTS){
        printf("[ERROR] Max endpoints reached");
        return;
    }
    Endpoint* newEndpoint = (Endpoint*) malloc(sizeof(Endpoint));
    newEndpoint->type = type;
    newEndpoint->path = strdup(path);
    newEndpoint->endpoint = endpoint;

    endpoints[endpointPointer++] = newEndpoint;
}
#include "endpoints.h"

Endpoint* endpoints[MAX_ENDPOINTS];
int endpointPointer = 0;

void destroyEndpoints(){
    while (endpointPointer > 0){
        free(endpoints[endpointPointer--]);
    }
}

void addEndpoint(REQUEST_TYPE type, char* path, char*(*endpoint)(Request request)){
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



char* callEndpoint(Request request){
    for (int i = 0; i < MAX_ENDPOINTS; ++i) {
        Endpoint* endpoint = endpoints[i];
        if(endpoint->type == request.requestType){
            if(strcmp(request.url.url, endpoint->path) == 0){
                return endpoint->endpoint(request);
            }
        }
    }
    return NULL;
}
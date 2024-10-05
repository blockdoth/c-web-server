#include "IO.h"

char* buildResponse(char* body){
    char* header = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
    int bodyLen = strlen(header) + strlen(body) + 1;
    char* response = (char*) malloc(bodyLen);
    snprintf(response, bodyLen, "%s%s", header, body);
    return response;
}

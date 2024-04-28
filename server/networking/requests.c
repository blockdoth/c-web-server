#include "requests.h"


char* getExtension(const char* path){
    int pathIndex = 0;
    while(*(path + pathIndex) != '\0'){
        pathIndex++;
    }
    while(*(path + pathIndex) != '.' && pathIndex > 0){
        pathIndex--;
    }
    if(pathIndex == 0){
        return NULL;
    }
    return strdup(path + pathIndex);
}


Extension parseExtension(char* url){
    if(strlen(url) == 0){
        return NVT;
    }
    char* extension = getExtension(url);
    Extension ext = NVT;
    if(strcmp(extension, ".html") == 0){
        ext = HTML;
    }else if(strcmp(extension, ".css") == 0){
        ext = CSS;
    }else if(strcmp(extension, ".txt") == 0){
        ext = TXT;
    }else if(strcmp(extension, ".md") == 0){
        ext = MD;
    }else if(strcmp(extension, ".ico") == 0){
        ext = ICO;
    }
    free(extension);
    return ext;
}

URL parseURL(char* buffer){
    while(*buffer != ' ' && *buffer != '\0'){
        buffer++;
    }
    buffer += 2;
    long size = 0;
    while(*(buffer + size) != ' '){
        //char c = *(buffer + endpos);
        size++;
    }

    char* url = strndup(buffer, size);
    int depth = 0;
    int depthIndex = 0;
    while(*(url + depthIndex) != '\0'){
        if(*(url + depthIndex) == '/'){
            depth++;
        }
        depthIndex++;
    }

    return (URL){url, depth + 1};
}


char* parseBody(const char* buffer){
    int bodyIndex = 0;
    while (*(buffer + bodyIndex) != '\0' && (*(buffer + bodyIndex) != '\n' || *(buffer + bodyIndex + 1) != '\r')) {
        bodyIndex++;
    }
    if (*(buffer + bodyIndex) == '\0') {
        return NULL;
    }
    const char* pointer = buffer + bodyIndex + 3;

    return strndup(pointer, strlen(pointer));
}

char* getURLChunk(URL url, int n){
    char* urlPointer = url.url;
    int urlIndex = 0;
    int chunkIndex = 0;
    while(n-- >= 0){
        urlIndex += chunkIndex;
        chunkIndex = 0;
        if(*(urlPointer + urlIndex + chunkIndex) == '/'){
            urlIndex++;
        }
        while(*(urlPointer + urlIndex + chunkIndex) != '/' && *(urlPointer + urlIndex + chunkIndex) != '\0') {
            chunkIndex++;
        }
    }
    return strndup(urlPointer + urlIndex, chunkIndex);
}




REQUEST_TYPE parseRequestType(char* buffer){
    if(strncmp(buffer, "GET", 3) == 0){
        return GET;
    }else if(strncmp(buffer, "POST", 4) == 0){
        return POST;
    }else if(strncmp(buffer, "PUT", 3) == 0){
        return PUT;
    }else if(strncmp(buffer, "DELETE", 6) == 0){
        return DELETE;
    } else {
        errno = EIO;
        return -1;
    }
}

Request parseRequest(char* buffer){
    URL url = parseURL(buffer);
    return (Request) {
            .requestType = parseRequestType(buffer),
            .url = url,
            .body = parseBody(buffer),
            .extension = parseExtension(url.url),
    };
}

char* match(const char* buffer, const char* pattern){
    while(*buffer != '\0'){ // Loop until end of buffer
        int patternIndex = 0;
        while(*(pattern + patternIndex) != '\0'){
            if(*(buffer + patternIndex) != *(pattern + patternIndex)){
                goto exitDoubleLoop;
            }
            patternIndex++;
        }
        int endLine = 0;
        while (*(buffer + endLine) != '\n'){
            endLine++;
        }
        return strndup(buffer + patternIndex + 1, endLine - 1);
        exitDoubleLoop:
        buffer++;
    }
    return NULL;
}

void destroyRequest(Request request){
    free(request.url.url);
    free(request.body);
}



void printRequest(Request request){
    char requestTypeStr[10];
    switch (request.requestType) {
        case GET:
            strcpy(requestTypeStr, "GET");
            break;
        case POST:
            strcpy(requestTypeStr, "POST");
            break;
        case PUT:
            strcpy(requestTypeStr, "PUT");
            break;
        case DELETE:
            strcpy(requestTypeStr, "DELETE");
            break;
        default:
            strcpy(requestTypeStr, "UNKNOWN");
    }
    char extension[10];
    switch (request.extension) {
        case HTML:
            strcpy(extension, ".html");
            break;
        case CSS:
            strcpy(extension, ".css");
            break;
        case TXT:
            strcpy(extension, ".txt");
            break;
        case MD:
            strcpy(extension, ".md");
            break;
        case ICO:
            strcpy(extension, ".ico");
            break;
        default:
            strcpy(extension, "UNKNOWN");
    }


    printf(" ├─[INFO] Request:\n");
    printf(" │\tType\t%s\n", requestTypeStr);
    printf(" │\tUrl\t%s\n", request.url.url);
    printf(" │\tExt\t%s\n", extension);
    printf(" │\tBody\t%s\n", request.body);
}

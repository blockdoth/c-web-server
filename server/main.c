#include <stdio.h>

#include <string.h>
#include <sys/socket.h>
#include "stdlib.h"
#include <netinet/in.h>
#include <pthread.h>
#include "stdbool.h"
#include <unistd.h>

#include "networking/endpoints.h"
#include "files/files.h"
#include "networking/requests.h"
#include "networking/IO.h"


typedef struct {
    int requestId;
    Pages* pages;
} ThreadArgs;

//Page* page = findPage(threadArgs->pages, request.url.url);
//if(page == NULL){
//printf(" ├─[ERROR] File not found\n");
//goto cleanUp;
//}

#define REQUEST_BUFFER_SIZE 10000
void* handleRequest(void* arg){
    ThreadArgs* threadArgs = (ThreadArgs*) arg;

    char* buffer = (char*) malloc(REQUEST_BUFFER_SIZE);
    ssize_t bytesReceived = recv(threadArgs->requestId, buffer, REQUEST_BUFFER_SIZE, 0);
    printf(" ├─[INFO] Bytes received\t%zd\n",bytesReceived);
    if(bytesReceived > 0){
        //printf("%s", buffer);
        Request request = parseRequest(buffer);
        printRequest(request);
        if(request.extension == ICO){
            printf(" ├─[ERROR] .ico is not supported\n");
        }else if (request.extension == NVT){
            printf(" ├─[ERROR] No path specified, no base yet\n");
        }
        else {
            char* page = callEndpoint(request);
            char* response = buildResponse(page);
            send(threadArgs->requestId, response, strlen(response),0);
            free(page);
            free(response);
            printf(" ├─[INFO] Send response\n");
        }
        destroyRequest(request);
    }
    close(threadArgs->requestId);
    free(buffer);
    free(arg);
    printf(" └─[INFO] Request handled\n");
    return NULL;
}



#define PORT 8080
int main() {
    Pages* pages = loadPages(HTMLPATH);
    printf("[INFO] Loaded %u pages\n", pages->pageCount);

    char** links = (char**) malloc(sizeof(char*) * 2);
    links[0] = "hello world";
    links[1] = "test test";
    char* test = replaceLinks(pages->pageFiles[0],links, 2);
    printf("%s", test);
//    for (int i = 0; i < ; ++i) {
//
//        addEndpoint(GET, ,);
//    }

    int socketFD;
    printf("[INFO] Starting socket\n");
    if((socketFD = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("[ERROR] Starting socket failed\n");
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Socket created\n");

    int yes = 1;
    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("[ERROR Failed to config socket]");
        exit(1);
    }
    struct sockaddr_in socketAddr;
    socklen_t socketAddrLen = sizeof(socketAddr);
    socketAddr.sin_family = AF_INET; //ipv4
    socketAddr.sin_port = htons(PORT);
    socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(socketFD, (struct sockaddr *)&socketAddr, sizeof(socketAddr)) < 0){
        perror("[ERROR] Failed to bind socket\n");
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Socket bound to port %d\n", PORT);
    if(listen(socketFD, 64) < 0){
        perror("[ERROR] Failed to start listener on socket\n");
        exit(EXIT_FAILURE);
    }
    printf("[INFO] Listener started\n");

    while (true){
        int requestId;
        pthread_t threadId;
        ThreadArgs* threadArgs = (ThreadArgs*) malloc(sizeof(ThreadArgs));

        if((requestId = accept(socketFD, (struct sockaddr *)&socketAddr, &socketAddrLen)) < 0){
            perror("[ERROR] Failed to accept\n");
            continue;
        }
        threadArgs->requestId = requestId;
        threadArgs->pages = pages;
        printf("[INFO] Request handler added\n");
        pthread_create(&threadId, NULL, handleRequest, threadArgs);
        pthread_detach(threadId);

    }
}

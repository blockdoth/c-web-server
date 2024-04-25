#include <stdio.h>

#include <string.h>
#include <sys/socket.h>
#include "stdlib.h"
#include <netinet/in.h>
#include <pthread.h>
#include "stdbool.h"
#include <unistd.h>

#include "files.h"
#include "requests.h"
#include "IO.h"


typedef struct {
    int requestId;
} PThreadArgs;


#define REQUEST_BUFFER_SIZE 10000
void* handleRequest(void* arg){
    PThreadArgs* threadArgs = (PThreadArgs*) arg;

    char* buffer = (char*) malloc(REQUEST_BUFFER_SIZE);
    ssize_t bytesReceived = recv(threadArgs->requestId, buffer, REQUEST_BUFFER_SIZE, 0);
    printf(" ├─[INFO] Bytes received\t%zd\n",bytesReceived);
    if(bytesReceived > 0){
        printf("%s", buffer);
        Request request = parseRequest(buffer);
        printRequest(request);
        if(request.extension == ICO){
            printf(" ├─[ERROR] .ico is not supported\n");
        }else if (request.extension == NVT){
            printf(" ├─[ERROR] No path specified, no base yet\n");
        }
        else {
            switch (request.requestType) {
                case GET:
//                for (int i = 0; i < request.url.depth; ++i) {
//                    char* chunk = getURLChunk(request.url, i);
//                    printf("%s\n", chunk);
//                }
                {
                    char* path = (char*) malloc(strlen(request.url.url) + 6);
                    strcpy(path, "../../");
                    strcat(path, request.url.url);
                    char* file = openFile(path);
                    free(path);
                    if(file == NULL){
                        printf(" ├─[ERROR] Could not find file \"%s\"\n", request.url.url);
                        free(file);
                        goto cleanUp;
                    }
                    char* response = sendResponse(file);
                    send(threadArgs->requestId, response, strlen(response),0);
                    printf(" ├─[INFO] Send response\n");
                    free(response);
                    free(file);
                    break;
                }
                default:
            }
        }
        cleanUp:
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
    //RawFiles* rawFiles = loadRawFiles();
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
        PThreadArgs* threadArgs = (PThreadArgs*) malloc(sizeof(threadArgs));

        if((requestId = accept(socketFD, (struct sockaddr *)&socketAddr, &socketAddrLen)) < 0){
            perror("[ERROR] Failed to accept\n");
            continue;
        }
        threadArgs->requestId = requestId;
        pthread_t threadId;
        printf("[INFO] Request handler added\n");
        pthread_create(&threadId, NULL, handleRequest, threadArgs);
        pthread_detach(threadId);

    }
}

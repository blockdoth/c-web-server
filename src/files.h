#ifndef WEBSITE_FILES_H
#define WEBSITE_FILES_H

#include <dirent.h>
#include <malloc.h>
#include "string.h"

#include "requests.h"

typedef struct {
    char* content;
    int length;
} RawFile;

typedef struct {
    RawFile** postfiles;
    int postCount;
    int bufferSize;
} RawFiles;

void rawFilesAppend(RawFiles* rawFiles, char* content, int length);
RawFiles* initRawFiles(int initalSize);
RawFiles* loadRawFiles();
char* openFile(char* url);
#endif //WEBSITE_FILES_H

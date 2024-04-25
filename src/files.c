#include "files.h"



char* serveFileFromURL(URL url){
    printf("%s", url.url);
    char* urlPointer = url.url;
    while(*urlPointer != '\0'){
        int chunkIndex = 0;
        while(*(urlPointer + chunkIndex) != '/'){
            chunkIndex++;
        }
        char* urlChunk = strndup(urlPointer, chunkIndex);

        printf("%s", urlChunk);
    }

    return NULL;
}



char* openFile(char* url){
    FILE* fp = fopen(url, "r");
    if(fp == NULL){
        return NULL;
    }
    fseek(fp,0, SEEK_END);
    int fileLength = (int) ftell(fp);
    fseek(fp,0,SEEK_SET);
    //printf("Opened %s of size %d\n", path, fileLength);
    char* fileContent = (char*) malloc(fileLength);
    fread(fileContent,1,fileLength,fp);
    fclose(fp);
    return fileContent;
}


void rawFilesAppend(RawFiles* rawFiles, char* content, int length){
    RawFile* rawfile = (RawFile*) malloc(sizeof (RawFile));
    rawfile->content = content;
    rawfile->length = length;
    rawFiles->postfiles[rawFiles->postCount++] = rawfile;
    if(rawFiles->postCount * sizeof(RawFile) > rawFiles->bufferSize){
        rawFiles->postfiles = realloc(rawFiles->postfiles, rawFiles->bufferSize * 2);
    }
}

RawFiles* initRawFiles(int initalSize){
    initalSize *= sizeof(RawFile);
    RawFiles* postFiles = (RawFiles*) malloc(sizeof(RawFiles));
    postFiles->postfiles = malloc(initalSize);
    postFiles->bufferSize = initalSize;
    postFiles->postCount = 0;
    return postFiles;
}

RawFiles* loadRawFiles(){
    DIR *directory;
    struct dirent *entry;
    directory = opendir("../../posts");
    if (directory == NULL) {
        perror("Unable to open directory");
        return NULL;
    }

    RawFiles* rawFiles = initRawFiles(10);

    int fileCount = 0;
    char basePath[] = "../../posts/";
    int baseLen = strlen(basePath);
    char* path = (char*) malloc(baseLen);
    strcpy(path, basePath);
    while ((entry = readdir(directory)) != NULL) {
        // Filter out "." and ".." entries
        if (entry->d_type == DT_REG) {
            strcpy(path + baseLen, entry->d_name);
            FILE* file = fopen(path, "r");
            if (file == NULL) {
                perror("Error opening file");
                return NULL;
            }
            fseek(file,0, SEEK_END);
            int fileLength = (int) ftell(file);
            fseek(file,0,SEEK_SET);
            printf("Opened %s of size %d\n", path, fileLength);
            char* fileContent = (char*) malloc(fileLength);
            fread(fileContent,1,fileLength,file);
            printf("%s\n\n",fileContent);
            rawFilesAppend(rawFiles, fileContent, fileLength);
            fclose(file);
            fileCount++;
        }
    }

    printf("Content\n%s\n", rawFiles->postfiles[1]->content);
    free(path);
    // Close the directory
    closedir(directory);
    return rawFiles;
}

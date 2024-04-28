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



char* openPage(char* url){
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


void appendPage(Pages* pages, char* content, int length, char* name){
    Page* rawfile = (Page*) malloc(sizeof (Page));
    rawfile->content = content;
    rawfile->name = strdup(name);
    rawfile->length = length;
    pages->pageFiles[pages->pageCount++] = rawfile;
    if(pages->pageCount * sizeof(Page) > pages->bufferSize){
        pages->pageFiles = realloc(pages->pageFiles, pages->bufferSize * 2);
    }
}

Pages* initPages(int initialSize){
    initialSize *= sizeof(Page);
    Pages* postFiles = (Pages*) malloc(sizeof(Pages));
    postFiles->pageFiles = malloc(initialSize);
    postFiles->bufferSize = initialSize;
    postFiles->pageCount = 0;
    return postFiles;
}

Page* findPage(Pages* pages, char* name){
    for (int i = 0; i < pages->pageCount; ++i) {
        Page* page = pages->pageFiles[i];
        if(strcmp(page->name, name) == 0){
            return page;
        }
    }
    return NULL;
}

void destroyPage(Page* page){
    free(page->content);
    free(page->name);
    free(page);
}


Pages* loadPages(char* basePath){
    DIR* directory = opendir(HTMLPATH);
    if (directory == NULL) {
        perror("Unable to open directory");
        return NULL;
    }
    Pages* pages = initPages(10);
    int fileCount = 0;
    struct dirent *entry;
    while ((entry = readdir(directory)) != NULL) {
        // Filter out "." and ".." entries
        if (entry->d_type == DT_REG) {
            int baseLen = strlen(basePath);
            char* path = (char*) malloc(baseLen + strlen(entry->d_name) + 1);
            strcpy(path, basePath);
            strcpy(path + baseLen, entry->d_name);
            FILE* file = fopen(path, "r");
            free(path);
            if (file == NULL) {
                perror("[ERROR] File not found");
                return NULL;
            }
            fseek(file,0, SEEK_END);
            int fileLength = (int) ftell(file);
            fseek(file,0,SEEK_SET);
            char* fileContent = (char*) malloc(fileLength + 1);
            fread(fileContent,1,fileLength,file);
            appendPage(pages, fileContent, fileLength, entry->d_name);
            fclose(file);
            printf("[INFO] Loaded %s of size %d\n", entry->d_name, fileLength);
            fileCount++;
        }
    }

    // Close the directory
    closedir(directory);
    return pages;
}

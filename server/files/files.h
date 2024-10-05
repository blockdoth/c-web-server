#ifndef WEBSITE_FILES_H
#define WEBSITE_FILES_H

#include <dirent.h>
#include <malloc.h>
#include "string.h"

#include "../networking/requests.h"

#define HTMLPATH "../../website/pages/"
#define RESOURCESPATH "../../website/resources/"

typedef struct {
    char* name;
    char* content;
    int length;
} Page;

typedef struct {
    Page** pageFiles;
    int pageCount;
    int bufferSize;
} Pages;
char* replaceLinks(Page* page, char** links, int linkCount);

void appendPage(Pages* pages, char* content, int length, char* name);
Pages* initPages(int initialSize);
Pages* loadPages(char* basePath);
void destroyPage(Page* page);

Page* findPage(Pages* pages, char* name);
char* openPage(char* url);
#endif //WEBSITE_FILES_H

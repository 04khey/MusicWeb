#ifndef musweb_utilityfuncs
#define musweb_utilityfuncs

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <regex.h> 

struct MUS_NODE {
    int NumLinks;
    struct MUS_NODE** Links;
    float* Weights;
    char* NiceName;
    char* FullURI;
    int FileID; //for serialisation
    

};
struct LIBRARY{
    int NumNodes;
    struct MUS_NODE** Songs;
};
struct MUS_NODE* musNodeFromURI(char* dir, char* fileName, int FileID);
struct LIBRARY* getSongsFromm3u(char* pathTom3u);
struct LIBRARY* getSongsFromDir(char* dirString);
void printLibrary(struct LIBRARY* library);
struct MUS_NODE* musNodeFromURI(char* dir, char* fileName, int FileID);


#endif
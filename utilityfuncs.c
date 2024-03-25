#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <regex.h>   //for identifying music files

char* AUDIO_FILE_MATCH_STRING = ".mp3"; //TODO: replace with real regex

typedef struct MUS_NODE {
    int NumLinks;
    struct MUS_NODE** Links;
    float* Weights;
    char* NiceName;
    char* FullURI;
    int FileID; //for serialisation
    

} MUS_NODE;

typedef struct LIBRARY{
    int NumNodes;
    struct MUS_NODE** Songs;
} LIBRARY;

//function headers
MUS_NODE* musNodeFromURI(char* dir, char* fileName);




LIBRARY* getSongsFromDir(char* dirString){

    regex_t regex;
    if (regcomp(&regex, AUDIO_FILE_MATCH_STRING, 0)) { 
        fprintf(stderr, "Regex Compilation failure\n");
        exit(1);
    }


    char command[261];
    strcpy(command, "ls ");
    strcat(command, dirString);//surely unsanitised user inupt is fine here *CLUELESS*

    char fileNameBuffer[255];

    int numFilesInDir = 0;

    FILE* pp = popen(command, "r");

    while(fgets(fileNameBuffer, sizeof(fileNameBuffer)/sizeof(fileNameBuffer[0]), pp) != NULL){
        //printf("%s", fileNameBuffer);

        int reti = regexec(&regex, fileNameBuffer, 0, NULL, 0);
        if (!reti) {
            printf("%s", fileNameBuffer);
            numFilesInDir++;
        }

    }
    printf("%d Audio files found.\n", numFilesInDir);


    LIBRARY* outLibrary = malloc(sizeof(LIBRARY));
    outLibrary->NumNodes = numFilesInDir;
    outLibrary->Songs = malloc(numFilesInDir * sizeof(MUS_NODE));


    //fseek(pp, 0, SEEK_SET); doesn't seem to work for popen();.
    pclose(pp);
    FILE* pp2 = popen(command, "r");


    int currentNode = 0;
    int doneFlag = 0;
    while((currentNode < numFilesInDir) && !doneFlag ){
        doneFlag = (fgets(fileNameBuffer, sizeof(fileNameBuffer)/sizeof(fileNameBuffer[0]), pp2) == NULL);
        int reti = regexec(&regex, fileNameBuffer, 0, NULL, 0);
        if (!reti) {
            outLibrary->Songs[currentNode] = musNodeFromURI(dirString, fileNameBuffer);
            currentNode++;
        }
    }

    pclose(pp2);
    return outLibrary;
}

MUS_NODE* musNodeFromURI(char* dir, char* fileName){ //TODO
    char URI[512];
    strcpy(URI, dir);
    strcat(URI, fileName);
    printf("adding %s", URI);
}


int main(int argc, char* argv[]){
    getSongsFromDir("/mnt/c/Users/Dmitry/Downloads/");
}
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
MUS_NODE* musNodeFromURI(char* dir, char* fileName, int FileID);

LIBRARY* getSongsFromm3u(char* pathTom3u){
    FILE* fp = fopen(pathTom3u, "r");
    
    

    fclose(fp);
}

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

    //fseek(pp, 0, SEEK_SET); doesn't seem to work for popen();.
    pclose(pp);
    FILE* pp2 = popen(command, "r");

    LIBRARY* outLibrary = malloc(sizeof(LIBRARY));
    outLibrary->NumNodes = numFilesInDir;
    outLibrary->Songs = malloc(numFilesInDir * sizeof(MUS_NODE));


    int currentNode = 0;
    int doneFlag = 0;
    while((currentNode < numFilesInDir) && !doneFlag ){
        doneFlag = (fgets(fileNameBuffer, sizeof(fileNameBuffer)/sizeof(fileNameBuffer[0]), pp2) == NULL);
        int reti = regexec(&regex, fileNameBuffer, 0, NULL, 0);
        if (!reti) {
            outLibrary->Songs[currentNode] = musNodeFromURI(dirString, fileNameBuffer, currentNode);
            currentNode++;
        }
    }

    pclose(pp2);
    return outLibrary;
}

void printLibrary(LIBRARY* library){
    printf("Songs in Library: %d\n", library->NumNodes);
    MUS_NODE* temp;
    for(int i=0;i<library->NumNodes;i++){
        temp = library->Songs[i];
        printf("---\nNode %d: %s\n", temp->FileID, temp->NiceName);
        printf("Num Links: %d\n", temp->NumLinks);
        printf("URI: %s\n", temp->FullURI);
    }
}

MUS_NODE* musNodeFromURI(char* dir, char* fileName, int FileID){ //TODO
    char URI[512];
    strcpy(URI, dir);
    strcat(URI, fileName);

    MUS_NODE* outNode = malloc(sizeof(MUS_NODE));
    outNode->NumLinks = 0;
    outNode->FileID = FileID;
    //don't need to initialise Links** since we'll do this later when we see numlinks=0
    //same for weights.
    outNode->NiceName = malloc((strlen(fileName))*sizeof(char));
    fileName[strlen(fileName)-1]='\0'; //trim \n
    strcpy(outNode->NiceName, fileName);
    //printf("copied %s", outNode->NiceName);
    outNode->FullURI = malloc((strlen(URI))*sizeof(char));
    URI[strlen(URI)-1]='\0'; //trim \n
    strcpy(outNode->FullURI, URI);
    //printf("copied %s", outNode->FullURI);
    return outNode;
}


int main(int argc, char* argv[]){
    LIBRARY l = *getSongsFromDir("/mnt/c/Users/Dmitry/Downloads/");
    printLibrary(&l);
}
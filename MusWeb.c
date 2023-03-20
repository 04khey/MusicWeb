#include <stdio.h>
#include <unistd.h> //for flag parsing. Necessary?
#include <getopt.h> //for flag parsing
#include <string.h>
#include <stdlib.h>

/**
* Will probably do
* DEPENDENCIES :
* cmus-remote
* cmus
* mpg123?
*/


/**TODO:
* Lazy track selection via cmus feedback 
* Add node to existing playlist
* Unicode support? ✓
* Detect nonexistent songs 
* Support for skipping
* interface with cmus
*/

//CONSTANTS
#define NAMEBUFFERSIZE 100
#define NUM_ENTRY_POINTS 8

//data structure for music node
typedef struct MUS_NODE {
    float* weights;
    struct MUS_NODE** links; //array of pointers to other musnodes
    int namelength;
    char* filename;

} MUS_NODE; 
typedef struct PLAYLIST {
    MUS_NODE* entrypoints[NUM_ENTRY_POINTS]; //up to 8 entry points into the playlist. Will be listed. 
    int nodecount;
    MUS_NODE** nodes; //pointer to array of contiguous pointers to mus_nodes. can't just array mus_nodes as variable size.
} PLAYLIST;



//FLAGS

int VISUALISE_MODE = 0;
char INPUT_FILE[NAMEBUFFERSIZE];
char OUTPUT_FILE[NAMEBUFFERSIZE];

static struct option const long_options[] = //see https://linux.die.net/man/3/getopt_long
  { //char *name, int has_arg, int *flag (pointer to load val into if flag true), int val  
    {"visualise", 0, &VISUALISE_MODE, 1},
    {NULL, 0, NULL, 0} //required convention. Acts as a terminator struct for the thing reading long_options.
  };



int load_opts(int argc, char* argv[]){
    int c;
    while ( (c = getopt_long(argc, argv, "i:o:", long_options, NULL)) != -1){
        switch(c){
            
            case 'i':
            strcpy(INPUT_FILE, optarg); break;
            case 'o':
            strcpy(OUTPUT_FILE, optarg); break;
        }
        
    }
    if(INPUT_FILE[0] == '\0' || OUTPUT_FILE[0] == '\0'){ //probably a standard way to do this with lib. Also -i test -o --visualise gives bad behaviour.
         fprintf (stderr, "Incorrect arguments. See man page.\n");
         return -1;
    }
    return 0;

}

PLAYLIST* readm3u(){
    //open file
    //determine number of mp3s (lines)
    FILE* fp = fopen(INPUT_FILE, "r"); //if no fp, return -1
    int numlines=0;
    if(fp == NULL){
        fprintf (stderr, "File %s is unreadable.\n", INPUT_FILE);
        exit(-1);
    }
    char c[NAMEBUFFERSIZE];
    while (fgets(c, NAMEBUFFERSIZE, fp)) {
        
        numlines += c[strlen(c)-1] == '\n';
        if(c[0] == '\n' || c[0] == '\0'){
            numlines--;
            printf("hit");
        }
    }
    printf("lines: %d\n", numlines);

    fseek(fp, 0, SEEK_SET);//go back to start

    int numchars[numlines];
    for(int i=0;i<numlines;i++){ //assuming no spaces between songs... :)
        fgets(c, NAMEBUFFERSIZE, fp);
        numchars[i] = strlen(c) + 1;
    }

    fseek(fp, 0, SEEK_SET);//go back to start

    int numfiles = numlines;
    PLAYLIST* pl = malloc(sizeof(PLAYLIST));

    MUS_NODE* lastnode;
    pl->nodes = malloc(numfiles * sizeof(MUS_NODE *));
    pl->nodecount = numfiles;

    for(int i=0;i<numfiles;i++){
        MUS_NODE* m = malloc(sizeof(MUS_NODE));

        if(i==0){
            m->links = calloc(1,  1 * sizeof(MUS_NODE**)); //allocating size here. 1 represents number of links this node has to others. Basic link so this. Will deep copy and reallocate to grow
            m->weights = calloc(1,  1 * sizeof(MUS_NODE**));
        } else if(i==numfiles){

        } else{
            m->links = calloc(1,  1 * sizeof(MUS_NODE**)); //allocating size here. 1 represents number of links this node has to others. Basic link so this. Will deep copy and reallocate to grow
            m->weights = calloc(1,  1 * sizeof(MUS_NODE**));
            lastnode->links[0] = m;
            lastnode->weights[0] = 1.0f;
            //m->links[0] = lastnode;
            //m->weights[0] = 1.0f;
        }
        m->filename = malloc((numchars[i]-1) * sizeof(char));
        char temp[numchars[i]];
        fgets(temp, NAMEBUFFERSIZE, fp);
        strncpy(m->filename, temp, numchars[i] -1);
        m->filename[numchars[i]-2]  = '\0';
        m->namelength = numchars[i] - 2;
        
        pl->nodes[i] = m;
        lastnode = m;
    }

    fclose(fp);
    return pl;
}

int main(int argc, char *argv[]) {
    if (load_opts(argc, argv)){
        return -1; //incorrect kwargs
    }
    //printf("input: %s, output:%s, visualise:%i\n",INPUT_FILE ,OUTPUT_FILE, VISUALISE_MODE);
    printf("sizeof(MUS_NODE): %lu\n", sizeof(MUS_NODE));
    PLAYLIST * pl = readm3u();
    //printf("sizeof(pl->nodes):%lu\n", pl->nodecount);
    for(int i=0;i<pl->nodecount;i++){
        if(i<pl->nodecount-1){
            printf("FILE %i: %s\nLink:%s\n", i, pl->nodes[i]->filename, pl->nodes[i]->links[0]->filename);
        } else{
            printf("FILE %i: %s\n", i, pl->nodes[i]->filename);
        }
    }
}
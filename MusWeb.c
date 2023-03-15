#include <stdio.h>
#include <unistd.h> //for flag parsing. Necessary?
#include <getopt.h> //for flag parsing
#include <string.h>

/**
* Will probably do
* DEPENDENCIES :
* cmus-remote
* cmus
* mpg123?
*/


//data structure for music node
typedef struct MUS_NODE {
    float weights[10];
    char filename[40];
    struct MUS_NODE* links[]; //array of pointers to other musnodes

} MUS_NODE; 
typedef struct PLAYLIST {
    MUS_NODE* entrypoints[8]; //up to 8 entry points into the playlist. Will be listed. 
    MUS_NODE nodes[];
} PLAYLIST;



//FLAGS
int VISUALISE_MODE = 0;
char INPUT_FILE[25];
char OUTPUT_FILE[25];

static struct option const long_options[] = //see https://linux.die.net/man/3/getopt_long
  { //char *name, int has_arg, int *flag (pointer to load val into if flag true), int val  
    {"visualise", 0, &VISUALISE_MODE, 1},
    {NULL, 0, NULL, 0} //convention. Acts as a terminator struct for the thing reading long_options.
  };



void load_opts(int argc, char* argv[]){
    int c;
    while ( (c = getopt_long(argc, argv, "io", long_options, NULL)) != -1){
        //printf("for %c, parsed %s\n", c, argv[optind]);
        switch(c){
            case 'i':
            strcpy(INPUT_FILE, argv[optind]); break;
            case 'o':
            strcpy(OUTPUT_FILE, argv[optind]); break;
        }
        
    }

}



int main(int argc, char *argv[]) {
    load_opts(argc, argv);
    printf("input: %s, output:%s, visualise:%i\n",INPUT_FILE ,OUTPUT_FILE, VISUALISE_MODE);
    printf("sizeof(MUS_NODE): %lu\n", sizeof(MUS_NODE));
}
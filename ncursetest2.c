#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h> //for flag parsing. Necessary?
#include <getopt.h> //for flag parsing
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>


//CONSTANTS
#define NAMEBUFFERSIZE 100

//data structure for music node
typedef struct MUS_NODE {
    float* weights;
    int numlinks;
    struct MUS_NODE** links; //array of pointers to other musnodes
    int namelength;
    char* filename;

} MUS_NODE; 
typedef struct PLAYLIST {
    int entrypointcount;
    MUS_NODE** entrypoints; //entry points into the playlist. Will be listed.
    int nodecount;
    MUS_NODE** nodes; //pointer to array of contiguous pointers to mus_nodes. can't just array mus_nodes as variable size.
} PLAYLIST;

//GLOBALS
PLAYLIST pl;

//FLAGS

int VISUALISE_MODE = 0;
char INPUT_FILE[NAMEBUFFERSIZE];
char OUTPUT_FILE[NAMEBUFFERSIZE];

static struct option const long_options[] = //see https://linux.die.net/man/3/getopt_long //this is a struct option
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
         fprintf (stderr, "Incorrect arguments. Usage: MusWeb -i infile.m3u -o outfile.\n");
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
    printw("lines: %d\n", numlines);
    refresh();

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
    pl->entrypointcount = 1;
    pl->entrypoints = malloc (1 * sizeof(MUS_NODE *));

    for(int i=0;i<numfiles;i++){
        MUS_NODE* m = malloc(sizeof(MUS_NODE));
        //probably could be more compact as a case switch..
        if(i==0){
            m->links = calloc(1,  1 * sizeof(MUS_NODE**)); //allocating size here. 1 represents number of links this node has to others. Basic link so this. Will copy and reallocate to grow
            m->numlinks = 1;
            m->weights = calloc(1,  1 * sizeof(MUS_NODE**));
            pl->entrypoints[0] = m;
            
        } else if(i==numfiles){ //why is this here?

        } else{
            m->links = calloc(1,  1 * sizeof(MUS_NODE**)); //allocating size here. 1 represents number of links this node has to others. Basic link so this. Will copy and reallocate to grow
            m->numlinks = 1;
            m->weights = calloc(1,  1 * sizeof(MUS_NODE**));
            lastnode->links[0] = m;
            lastnode->weights[0] = 1.0f;
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

MUS_NODE* selectEntryNode(int height, int width){
    printf( "\e[2J" );
    printf( "OPTIONS:\n");
    for(int i=0;i<pl.entrypointcount;i++){
        printw("[%i]:%s\n",i,pl.entrypoints[i]->filename);
    }
    for(int j=pl.entrypointcount;j<height-1;j++){
        putchar('\n');
    }
    refresh();
    int in;

    scanf("%5i", &in);
    while(in>(pl.entrypointcount -1) || in < 0){
        refresh();
        printw("SELECTION OUT OF RANGE. TRY AGAIN.\n");
        for(int i=0;i<pl.entrypointcount;i++){
            printw("[%i]:%s\n",i,pl.entrypoints[i]->filename);
        }
        for(int j=pl.entrypointcount;j<height-2;j++){
            putchar('\n');
        }   
        printw("%5i", &in);
    }
    printw("%i\n", in);
    return pl.entrypoints[in];
}

int FILE_EXISTS(char * filename){ //should check a specified music directory
    return access(filename, F_OK) == 0;
}

void createAndAppendNode(MUS_NODE * parent, char * filename, float parentweight){
    //assumes playlist exists

    //add weight to parent
    float * tempweights = parent->weights;
    parent->weights = malloc(sizeof(float) * (parent->numlinks + 1));
    for(int i=0;i<parent->numlinks;i++){
        parent->weights[i] = tempweights[i];
    }
    parent->weights[parent->numlinks] = parentweight;
    free(tempweights);

    int nodeExists = 0;
    MUS_NODE * m;

    //FIRST: Check if music file already in pl :)
    for(int i=pl.nodecount;i>0;i--){
        if(!strcmp(pl.nodes[i-1]->filename, filename)){
            nodeExists = i;
            break;
        }
    }
    if(nodeExists){
        m = pl.nodes[nodeExists-1];
    } else {
        m = malloc(sizeof(MUS_NODE));
        m->filename = filename;
        m->numlinks = 0;
    }

    //add node pointer to parent
    MUS_NODE ** templinks = parent->links;
    parent->links = malloc((parent->numlinks + 1) * sizeof(MUS_NODE*));
    for(int i=0;i<parent->numlinks;i++){
        parent->links[i] = templinks[i];
    }
    parent->links[parent->numlinks] = m;
    parent->numlinks++;

    //append m to playlist so it doesn't get free'd once this scope closes
    MUS_NODE ** tempnodes = pl.nodes; 
    pl.nodes = malloc((pl.nodecount + 1) * sizeof(MUS_NODE *));
    for(int i=0;i<pl.nodecount;i++){
        pl.nodes[i] = tempnodes[i];
    }
    pl.nodes[pl.nodecount] = m;
    free(tempnodes);
    pl.nodecount++;

}

void traverseWeb(){
    /*display current song
    * display links and weights
    * Take input for next
    * OPTIONS: 
    * Enter at entrypoint
    * Enter at specific song (enumerate or type name)
    * 
    */
    struct winsize ws;
    ioctl( 0, TIOCGWINSZ, &ws );
    int width = ws.ws_col;
    int height = ws.ws_row;
    //printf("\e[0;32m"); //colour code green ANSI (https://gist.github.com/RabaDabaDoba/145049536f815903c79944599c6f952a)
    printw( "\e[2J" ); //clear term
    for(int i=0;i<width;i++){
        putchar('=');
    } putchar('\n');

    printw("\nSelect Marking Start Node:\n[0]: %s\n[1]: Select from EntryNodes\n[2]: Enter exact file string (plus terminator)\n", pl.nodes[0]->filename);
    refresh();
    //6
    for(int i=0;i<height-7;i++){
        putchar('\n');
    }
    //printf("term width: %i chars\nterm height: %i chars\n", width, height);

    MUS_NODE* currnode;

    char in[40];
    scanf("%39s", in);
    switch(in[0]){
        case '0': currnode = pl.nodes[0];
        break;
        case '1': currnode = selectEntryNode(height, width);
        break;
        case '2': //TODO
        exit(-9001);
        break;
    }

    //we will use this later. Moved here to fix stack-use-after-scope.
    char toadd[NAMEBUFFERSIZE]; //should really be NAMEBUFFERSIZE
    strcpy(toadd, "e");

    //now we have currnode
    while (strcmp(in, "X")){
        printf("CURRENT NODE: %s\nX:exit\na:add child\ne:edit children\nm:mark as entrypoint\n\n[SELECT #]:Song(Relative Weight)\n", currnode->filename);
        for(int i=0;i<currnode->numlinks;i++){
            printf("[%i]:%s(%f)\n",i, currnode->links[i]->filename, currnode->weights[i]);
        }
        for(int j=currnode->numlinks;j<height-8;j++){
            putchar('\n');
        }
        char thing[10];
        char namebufsizeasstring[10];
        sprintf(namebufsizeasstring, "%d", NAMEBUFFERSIZE);

        strcpy(thing, "%"); //e.g. %40s
    
        strcat(thing, namebufsizeasstring);
        strcat(thing, "s");

        scanf(thing, in);
        int c = atoi(in); //atoi is unsafe
        if(c || !strcmp(in, "0")){
            currnode = currnode->links[c];
        } else{
            if(!strcmp(in, "a")){
                printw( "\e[2J" ); //clear term
                printw("Appending to node: %s\n", currnode->filename);
                printw("Current children:\n");
                for(int i=0;i<currnode->numlinks;i++){
                    printf("[%i]:%s(%f)\n",i, currnode->links[i]->filename, currnode->weights[i]);
                }
                for(int j=currnode->numlinks;j<height-4;j++){
                    putchar('\n');
                }
                printw("File URI to add:\n");
                
                char temp[40];
                double toweight;
                refresh();
                do{
                fgets(toadd, NAMEBUFFERSIZE - 1, stdin);
                
                if ((strlen(toadd) > 0) && (toadd[strlen (toadd) - 1] == '\n'))
                toadd[strlen (toadd) - 1] = '\0';

                if(FILE_EXISTS(toadd)){
                    //printf("FILE EXISTS!!!!!1\n");
                    printw("enter desired relative weight:\n");
                    refresh();
                    scanf("%39s", temp);
                    toweight = atof(temp);
                    createAndAppendNode(currnode, toadd, toweight);
                    }
                } while(!FILE_EXISTS(toadd));
            }

            }
        }
    }


void setupScreen(){
    initscr();
    cbreak();
    noecho(); //don't echo input chars
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE); //don't echo arrow keys
}

void drawInitialScreen(){
    //addstr("Hello, World!\nq:quit");
    pl = *readm3u();
    refresh();

    int stayinLoop=1;
    char c = 'j';
    while(stayinLoop){
        refresh();
        c = getch();
        switch(c){
            case 'q':
                stayinLoop=0;
                
                break;
            default:
                clear();
                printw("Last Character pressed: %c\n", c);
            break;


    }
    
    }
    endwin();
    
}


int main(int argc, char *argv[]) {
        
    setupScreen();
    drawInitialScreen();
    return EXIT_SUCCESS;
}


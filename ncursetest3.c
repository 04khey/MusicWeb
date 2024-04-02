#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h> 
#include <getopt.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

//header file stuff
#include "utilityfuncs.h"
#include "UIthing.h"
typedef struct LIBRARY LIBRARY;
typedef struct MUS_NODE MUS_NODE;
typedef struct PLAYLIST PLAYLIST;

//useful macro
#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

/*
    Results of the exectest were: 
    to run in any term: screen -d -m cmus;sleep 0.5;cmus-remote -q song1.mp3; cmus-remote -p
    to kill:            pkill cmus
    (assumes only 1 cmus instance, computer can launch cmus in 0.5s) 
*/

char* sickASCII[] = {
" __  __         __        __   _",
"|  \\/  |_   _ __\\ \\      / ___| |__",
"| |\\/| | | | / __\\ \\ /\\ / / _ | '_ \\",
"| |  | | |_| \\__ \\\\ V  V |  __| |_) |",
"|_|  |_|\\__,_|___/ \\_/\\_/ \\___|_.__/"
};

//headers from this file
void writesickASCII(WINDOW *win);
void listOptions(WINDOW *win);
PLAYLIST* playlistFromDir();

//globally useful
int WIDTH;
int HEIGHT;
LIBRARY* passedLibrary; //will need to be referred to in playlist editing/creation.

//https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/windows.html lol
int main(int argc, char *argv[]) 
{
    
    //system("screen -d -m cmus;sleep 0.1;cmus-remote -q song1.mp3; cmus-remote -p");//REMOVE ME 1 of 3
    
    //boilerplate
    WINDOW* mainwin = initscr();
    cbreak();
    noecho(); //don't echo input chars
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE); //don't echo arrow keys
    refresh(); //not sure why this is needed but it is
    
    //variables for later
    WINDOW *titleWin; 
    WIDTH = COLS;//just use COLS
    HEIGHT = LINES;//just use ROWS

    if(WIDTH < 40 || HEIGHT < 13) //tweak these values based on testing.
    {
        printw("too small! Press any key to exit.\n");
        getch();
        endwin();
        //system("pkill cmus"); //REMOVE ME 2 OF 3
        return 0;
    }


    titleWin = newwin(7,WIDTH,0,0);//height, width, starty, startx
    wborder(titleWin, '#','#','#','#','#','#','#','#');
    wrefresh(titleWin);

    WINDOW *bigWin = newwin(HEIGHT-7,WIDTH,7,0);

    refresh();
    
    listOptions(bigWin);
    writesickASCII(titleWin);
    char selection = 'z';
    while(selection != 'x'){
        selection = getch();
        switch (selection)
        {
        case '2':
            mvwprintw(stdscr,HEIGHT-1,0, "do thing now            ");
            playlistFromDir();
            wrefresh(bigWin);

            //TEST
            mvwprintw(stdscr,0,0, "First song in library: %s\nNum songs in library:%d", passedLibrary->Songs[0]->FullURI, passedLibrary->NumNodes);
            wrefresh(stdscr);

            getch();
            selection = 'x';
            break;
        
        default:
            mvwprintw(stdscr,HEIGHT-1,0, "Err: %c not recognised     ", selection);
            wrefresh(bigWin);
            break;
        }
        
    }

    delwin(titleWin);
    endwin();
   
    //system("pkill cmus"); //REMOVE ME 3 of 3 (?)
    return 0;
}

void writesickASCII(WINDOW *win){
    curs_set(0); //hide cursor. 0=hide,1=block,2=flashing block
    int tempx;
    int tempy;
    //getmaxyx(win, tempy, tempx);
    //wprintw(win, "max x, y: %d %d", tempx, tempy);

    int offset = (WIDTH/2) - (strlen(sickASCII[0]))/2 -2;

    for(int i=0;i<sizeof(sickASCII)/sizeof(sickASCII[0]);i++){
        mvwprintw(win, i+1,offset,"%s", sickASCII[i]);
    }


    wrefresh(win);
    //curs_set(1); //probably don't want this
}

char* menuOptions[] = {
"[0]:Edit existing playlist",
"[1]:Create playlist from m3u file (-i /path/to/playlist.m3u)",
"[2]:Create playlist from directory (-d /path/to/dir/)",
"[3]:Play playlist (-p /path/to/playlist.mw)",
"[4]:Visualise playlist",
"[5] Playlist to m3u",
"[x] Exit"
};

void listOptions(WINDOW *win){
    int temp_width;
    int temp_height;
    getmaxyx(win, temp_height, temp_width);
    for(int i=0;i<sizeof(menuOptions)/sizeof(menuOptions[0]);i++){
        mvwprintw(win,i,0, "%s", menuOptions[i]);
    }

    wrefresh(win);

}

MUS_NODE* getUserEntryNode(LIBRARY* lib){
    wclear(stdscr);

    //colours test
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    
    mvwprintw(stdscr, 0,0,"SELECT START NODE (up/down to move highlight, enter to select)");
    wrefresh(stdscr);
    WINDOW* searchResults = newwin(HEIGHT-1,WIDTH,1,0); //int nlines, int ncols, int begin_y, int begin_x
    WINDOW* searchBar = newwin(1,WIDTH,HEIGHT-1,0);
    
    /*
    int termHeight;
    int termWidth;
    getmaxyx(searchResults, termHeight, termWidth);*/

    char searchBuffer[255];
    strcpy(searchBuffer, "");


    int highlightLine =0;
    int input = 'x';
    char inAsChar;
    noecho();

    keypad(searchBar, TRUE);

    MUS_NODE* outNode;

    while(69){ 

    if(   (doSearchWinIteration(searchBar, searchResults, searchBuffer, &highlightLine, lib, &outNode)) == '\n'){
        return outNode;
    }

    //wrefresh(stdscr);
    
    }
    

    

    //wrefresh(searchBar);
    //wgetch(searchResults);
}
typedef struct NODESTACK{
    MUS_NODE** stackBase;
    int stackPointer; //points above top element of stack
    //int maxSize; //=NELEMS(stackBase)

} NODESTACK;

NODESTACK* push(MUS_NODE* toPush, NODESTACK* stack){ //MUS_NODE** stackBase, int stackPointer, int maxSize
    int maxOldSize=NELEMS(stack->stackBase);
    if(stack->stackPointer==maxOldSize-1){ //double stack size
        NODESTACK* newStack = malloc(sizeof(NODESTACK));
        newStack->stackBase = malloc(maxOldSize*2*sizeof(MUS_NODE*));
        for(int i=0;i<maxOldSize;i++){
            newStack->stackBase[i]=stack->stackBase[i];
        }
        newStack->stackBase[maxOldSize]=toPush;
        newStack->stackPointer=stack->stackPointer+1;
        //newStack->maxSize=maxOldSize*2;
        free(stack);
        return newStack;
    } else {
        stack->stackBase[stack->stackPointer] = toPush;
        stack->stackPointer++;
        return stack;
    }

}
MUS_NODE* peek(NODESTACK* stack){ //will crash if stackPointer <= 0. This is intentional.
    // if(stack->stackPointer>0){
        return stack->stackBase[stack->stackPointer];
    //} else{
        //idk. error?
    //}
}
MUS_NODE* pop(NODESTACK* stack){
    MUS_NODE* out; 
    out=peek(stack);
    stack->stackPointer--;
    return out;
}
int isStackEmpty(NODESTACK* stack){
    return stack->stackPointer==0;
}

void weightToString(char* bufferToWrite, float weight){ //fix later.
    snprintf(bufferToWrite, 12, "%.2f", weight);
}


PLAYLIST* editPlayList(PLAYLIST* inList, LIBRARY* lib){
    /*
        Prev Node: ...
        Current node: ...

    */

   wrefresh(stdscr);
   mvwprintw(stdscr,0,0,"%d %s", inList->NumEntryNodes, inList->EntryNodes[0]->NiceName);
   wrefresh(stdscr);

   int tooltipsize = 4;

    //divide width in 3.
    //headers        currNode |  searchBar | currWeight ■■▨□□□□□□□ 0.25
    WINDOW* currNode = newwin(1,WIDTH/3,0,0);
    WINDOW* searchBar = newwin(1,WIDTH-(2*WIDTH/3)-1,0,WIDTH/3);
    WINDOW* currWeight = newwin(1,WIDTH/3,0,(2*WIDTH/3));
    //static strip   navigation history |  search results | connected nodes:
    WINDOW* navHistory = newwin(1,WIDTH/3,1,0);
    WINDOW* searchReslts = newwin(1,WIDTH-(2*WIDTH/3)-1,1,WIDTH/3); //int nlines, int ncols, int begin_y, int begin_x
    WINDOW* connNodes = newwin(1,WIDTH/3,1,(2*WIDTH/3));
    mvwprintw(navHistory,0,0,"Navigation History:");
    mvwprintw(searchReslts,0,0,"Search results:");
    mvwprintw(connNodes,0,0,"Connected nodes:");
    wrefresh(navHistory);
    wrefresh(searchReslts);
    wrefresh(connNodes);

    //big panes
    WINDOW* navStack = newwin(HEIGHT-tooltipsize-2,WIDTH/3,2,0); //int nlines, int ncols, int begin_y, int begin_x
    WINDOW* searchResultsWin = newwin(HEIGHT-tooltipsize-2,WIDTH-(2*WIDTH/3)-1,2,WIDTH/3); //int nlines, int ncols, int begin_y, int begin_x
    WINDOW* currNodeWeights = newwin(HEIGHT-tooltipsize-2,WIDTH/3,2,(2*WIDTH/3)); //int nlines, int ncols, int begin_y, int begin_x
    WINDOW* tooltips = newwin(tooltipsize,WIDTH,HEIGHT-tooltipsize,0); //int nlines, int ncols, int begin_y, int begin_x

    const char* tooltipString[3] = { //need a 3x3xn matrix for row, col#, string
    "x:finish editing playlist\nEnter:add node with current weight", 
    "Ctrl+w: reduce weight by weightstep\nAlt+s:enter new weight", 
    "Alt+w: increase weight by weightstep"};
    char* toolS1 = "x:finish editing playlist";
    mvwprintw(tooltips, 0,0,"%s",toolS1);
    mvwprintw(tooltips, 0,WIDTH/3,"%s",tooltipString[1]);
    mvwprintw(tooltips, 0,(2*WIDTH/3),"%s",tooltipString[2]);
    wrefresh(tooltips);

    //probs not necessary
    keypad(searchBar, TRUE);
    

    //vars for lookup
    //for left=undo
    NODESTACK* nodesVisited = malloc(sizeof(NODESTACK));
    nodesVisited->stackBase = malloc(20*sizeof(MUS_NODE*));
    nodesVisited->stackPointer=0;
    int searchHighlightColumn =0;
    //for right=redo?
    //not yet...
    
    int finishedEditing = 0;
    char searchString[255];
    strcpy(searchString, "");
    //LIBRARY* searchResults = malloc(sizeof(LIBRARY));
    //searchResults = searchLibrary(lib, searchString);
    float currentWeight = 1.0f;
    char weightAsString[12];
    weightToString(weightAsString, currentWeight);

    MUS_NODE* currentNode = inList->EntryNodes[0];

    MUS_NODE* toAddWeightTo = malloc(sizeof(MUS_NODE)); //needed for searchbar

    int input;
   

    while(!finishedEditing){
        //print out headers
        mvwprintw(currNode, 0,0,"Current node: %s", currentNode->NiceName);
        wrefresh(currNode);
        mvwprintw(currWeight, 0,0,"Current weight: %s", weightAsString);
        wrefresh(currWeight);
        mvwprintw(searchBar, 0,0,"Search:");
        wrefresh(searchBar);
        //print out history stack
        for(int i=0;i<nodesVisited->stackPointer;i++){
            mvwprintw(navStack, i,0, "%s", nodesVisited->stackBase[i]->NiceName);
        }
        wrefresh(navStack);
        //print out search results
        /*
        searchResults = searchLibrary(lib, searchString);
        for(int i=0;i<searchResults->NumNodes;i++){
            mvwprintw(searchResultsWin, i,0,"%s", searchResults->Songs[i]->NiceName);
        }
        wrefresh(searchResultsWin);*/
        
        //WHY MUST THIS BE HERE TO SHOW?!
        mvwprintw(tooltips, 0,0,"%s",toolS1);
        mvwprintw(tooltips, 0,WIDTH/3,"%s",tooltipString[1]);
        mvwprintw(tooltips, 0,(2*WIDTH/3),"%s",tooltipString[2]);
        wrefresh(tooltips); 

         input=doSearchWinIteration(searchBar, searchResultsWin, searchString, &searchHighlightColumn, lib, &toAddWeightTo);
         
        //print out current node connections
        for(int i=0;i<currentNode->NumLinks;i++){
            mvwprintw(currNodeWeights,i,0,"[%3f] %s",currentNode->Weights[i],currentNode->Links[i]->NiceName);
        }
        wrefresh(currNodeWeights);
        
        mvwprintw(tooltips,2,2,"ynot");
                 wrefresh(tooltips);
         
        switch(input){
            case KEY_LEFT:
            case 59:
            //check stack not empty
            if(!isStackEmpty(nodesVisited)){
                currentNode = pop(nodesVisited);
                mvwprintw(tooltips,2,2,"curn: %s",currentNode->NiceName);
                wrefresh(tooltips);
                //push(redoStack, currentNode);
            } else{
                 mvwprintw(tooltips,2,2,"ynot");
                 wrefresh(tooltips);
            }
            //...
            break;
            case KEY_RIGHT:
            case 58:
            //check there is a valid result
            if((searchLibrary(lib, searchString)->NumNodes)>0){
                currentNode=searchLibrary(lib, searchString)->Songs[searchHighlightColumn];
                push(currentNode, nodesVisited);
            }
            break;
            default:
            mvwprintw(tooltips,2,2,"key pressed: %d  ",input);
            wrefresh(tooltips);
            break;
        }

        //if tooltip hide pressed, clear tooltips...
        
        
    }
    
}

PLAYLIST* playlistFromDir(){
    PLAYLIST* outList = malloc(sizeof(PLAYLIST));
    clear();
    mvprintw(0,0, "enter music directory\n");

    echo();
    curs_set(1); 
    refresh();

    char dirBuffer[255];
    getstr(dirBuffer);

    if(dirBuffer[strlen(dirBuffer)-1] != '/'){ //ls lets one get away with this, but we need it to be just so.
        dirBuffer[strlen(dirBuffer) ] ='/';
        dirBuffer[strlen(dirBuffer) ] ='\0';
    }
    //mvprintw(2,0,"%s", dirBuffer);
    //refresh();
    noecho();
    curs_set(0); //hide cursor. 0=hide,1=block,2=flashing block
    LIBRARY* dirlib = getSongsFromDir(dirBuffer);
    passedLibrary = dirlib;
    
    printw("Songs in Library: %d\n", dirlib->NumNodes);
    MUS_NODE* temp;
    for(int i=0;i<dirlib->NumNodes;i++){
        temp = dirlib->Songs[i];
        printw("---\nNode %d: %s\n", temp->FileID, temp->NiceName);
        printw("Num Links: %d\n", temp->NumLinks);
        printw("URI: %s\n", temp->FullURI);
    }
    refresh();

    MUS_NODE* EntryNode = getUserEntryNode(dirlib);

    outList->NumEntryNodes=1;
    outList->EntryNodes=malloc(1*sizeof(MUS_NODE*));
    outList->EntryNodes[0] = EntryNode;
    outList->NumNodesInPlaylist=1;
    outList->NodesInPlayList=malloc(1*sizeof(MUS_NODE*));
    outList->NodesInPlayList[0]=EntryNode;
    outList->playlistName = malloc(2*sizeof(char));
    outList->playlistName= "";

    wclear(stdscr);

    editPlayList(outList, passedLibrary);

    printw("Node selected: %s", outList->EntryNodes[0]->FullURI);
    
    wgetch(stdscr);

    return outList;
}
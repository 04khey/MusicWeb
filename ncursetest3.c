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
typedef struct LIBRARY LIBRARY;
typedef struct MUS_NODE MUS_NODE;
typedef struct PLAYLIST PLAYLIST;

/*
    Results of the exectest were: 
    to run in any term: screen -d -m cmus;sleep 0.5;cmus-remote -q song1.mp3; cmus-remote -p
    to kill:            pkill cmus 
*/

char* sickASCII[] = {
" __  __         __        __   _",
"|  \\/  |_   _ __\\ \\      / ___| |__",
"| |\\/| | | | / __\\ \\ /\\ / / _ | '_ \\",
"| |  | | |_| \\__ \\\\ V  V |  __| |_) |",
"|_|  |_|\\__,_|___/ \\_/\\_/ \\___|_.__/"
};

void writesickASCII(WINDOW *win);
void listOptions(WINDOW *win);
void playlistFromDir();

//globally useful
int WIDTH;
int HEIGHT;

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
    wclear(stdscr);//doesn't work?

    //colours test
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_GREEN);
    
    mvwprintw(stdscr, 0,0,"SELECT START NODE (up/down to move highlight, enter to select)");
    wrefresh(stdscr);
    WINDOW* searchResults = newwin(HEIGHT-1,WIDTH,1,0); //int nlines, int ncols, int begin_y, int begin_x
    WINDOW* searchBar = newwin(1,WIDTH,HEIGHT-1,0);
    
    int termHeight;
    int termWidth;
    getmaxyx(searchResults, termHeight, termWidth);

    char searchBuffer[255];
    strcpy(searchBuffer, "");


    int selected = 0;
    int highlightLine =0;
    int input = 'x';
    char inAsChar;
    noecho();

    keypad(searchBar, TRUE);

    LIBRARY* results;
    //results->NumNodes=0;

    results = searchLibrary(lib, searchBuffer);


    while(!selected){ //chars 32-126 inclusive allowed

        

        mvwprintw(searchBar, 0, 0, "Search: %s          %d", searchBuffer, input);
        wrefresh(searchBar);
        input = wgetch(searchBar);
        switch (input)
        {
        case KEY_BACKSPACE:
            mvwprintw(searchResults, 0, 0, "HIT!: %s          ", searchBuffer);
            searchBuffer[strlen(searchBuffer)-1]='\0';
        break;
        case KEY_UP:
        if(highlightLine==0){

        } else {
            highlightLine--;
        }
        break;
        case KEY_DOWN:
        if(highlightLine==results->NumNodes-1){

        } else {
            highlightLine++;
        }
        break;
        default:
        if(input>31 && input < 127){
            inAsChar = (char) input;
            mvwprintw(searchResults, 0, 0, "HIT!: %d          ", input);
            strncat(searchBuffer, &inAsChar, 1);
        } else{
            input = '\0';
        }
        break;
        }
        wclear(searchResults);

        
        results = searchLibrary(lib, searchBuffer);

        if(results->NumNodes<=highlightLine){
            highlightLine=0;
        }

        for(int i=0;i<results->NumNodes;i++){
            if(i==highlightLine){
                wattron(searchResults,COLOR_PAIR(1));
                attron(A_BLINK);
            }
            mvwprintw(searchResults,i,0, "%s",results->Songs[i]->NiceName);
            if(i==highlightLine){
                wattroff(searchResults,COLOR_PAIR(1));
            }
        }
        wrefresh(searchResults);
    }
    

    

    wrefresh(searchBar);
    wgetch(searchResults);
}

void playlistFromDir(){
    clear();
    mvprintw(0,0, "enter music directory\n");

    echo();
    curs_set(1); 
    refresh();

    char dirBuffer[255];
    getstr(dirBuffer);
    //mvprintw(2,0,"%s", dirBuffer);
    //refresh();
    noecho();
    curs_set(0); //hide cursor. 0=hide,1=block,2=flashing block
    LIBRARY* dirlib = getSongsFromDir(dirBuffer);
    
    printw("Songs in Library: %d\n", dirlib->NumNodes);
    MUS_NODE* temp;
    for(int i=0;i<dirlib->NumNodes;i++){
        temp = dirlib->Songs[i];
        printw("---\nNode %d: %s\n", temp->FileID, temp->NiceName);
        printw("Num Links: %d\n", temp->NumLinks);
        printw("URI: %s\n", temp->FullURI);
    }
    refresh();

    getUserEntryNode(dirlib);
    //WINDOW *searchBar = newwin(HEIGHT-7,WIDTH,7,0);
}
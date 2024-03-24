#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h> 
#include <getopt.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

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
    //char c = getch();
    
    listOptions(bigWin);
    writesickASCII(titleWin);


    delwin(titleWin);
    endwin();
   
    //system("pkill cmus"); //REMOVE ME 3 of 3 (?)
    return 0;
}

void writesickASCII(WINDOW *win){
    curs_set(0); //hide cursor. 0=hide,1=block,2=flashing block
    int tempx;
    int tempy;
    getmaxyx(win, tempy, tempx);
    wprintw(win, "max x, y: %d %d", tempx, tempy);

    int offset = (WIDTH/2) - (strlen(sickASCII[0]))/2 -2;

    for(int i=0;i<sizeof(sickASCII)/sizeof(sickASCII[0]);i++){
        mvwprintw(win, i+1,offset,"%s", sickASCII[i]);
    }


    wrefresh(win);
    getch();
    //curs_set(1); //probably don't want this
}

char* menuOptions[] = {
"[0]:Edit existing playlist",
"[1]:Create playlist from m3u file",
"[2]:Create playlist from directory",
"[3]:Play playlist",
"[4]:Visualise playlist",
"[5] Playlist to m3u"
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
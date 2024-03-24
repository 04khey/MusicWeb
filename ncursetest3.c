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

//globally useful
int WIDTH;
int HEIGHT;


//https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/windows.html lol
int main(int argc, char *argv[]) 
{
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

    if(WIDTH < 40 || HEIGHT < 7) //tweak these values based on testing.
    {
        printw("too small! Press any key to exit.\n");
        getch();
        endwin();
        return 0;
    }


    titleWin = newwin(7,WIDTH,0,0);//height, width, starty, startx
    wborder(titleWin, '#','#','#','#','#','#','#','#');
    wrefresh(titleWin);



    refresh();
    char c = getch();
    
    writesickASCII(titleWin);

    delwin(titleWin);
    endwin();
    return 0;
}

void writesickASCII(WINDOW *win){
    int tempx;
    int tempy;
    getmaxyx(win, tempy, tempx);
    wprintw(win, "max x, y: %d %d", tempx, tempy);

    int offset = (WIDTH/2) - (strlen(sickASCII[0]))/2;

    for(int i=0;i<sizeof(sickASCII)/sizeof(sickASCII[0]);i++){
        mvwprintw(win, i+1,offset,"%s", sickASCII[i]);
    }


    wrefresh(win);
    getch();
}
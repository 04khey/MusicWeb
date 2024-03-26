#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h> 
#include <getopt.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

//list of acceptable keys to use for combos (I hope)
const char ctrla = 1;
const char ctrle = 5;
const char ctrlf = 6;
const char ctrlg = 7;
const char ctrlr = 18;
const char ctrlt = 20;
const char ctrlv = 22;
const char ctrlw = 23;
const char ctrlx = 24;

int main(int argc, char *argv[]) 
{
    //boilerplate
    WINDOW* mainwin = initscr();
    cbreak();
    nonl(); //catch enter
    noecho(); //don't echo input chars
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE); //don't echo arrow keys
    refresh(); //not sure why this is needed but it is


    char inkeys[2];

    while(69){
        //getnstr(inkeys, 2);
        inkeys[0] = getch();
        //if(inkeys[0] == KEY_HOME
        //inkeys[1] = getch();
        
       
        if(inkeys[0] >0 && inkeys[0] < 27){
            printw("Ctrl+%c", 64 + inkeys[0] );
        } else if(inkeys[0] == 27){
            inkeys[1] = getch();
            printw("Alt + %c", inkeys[1]);
        } else {
                printw("Keys pressed: %c ", inkeys[0]);
                }
        refresh();
    }

    

    endwin();

    return 0;
}
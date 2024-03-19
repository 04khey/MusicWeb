#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

void setupScreen(){
    initscr();
    cbreak();
    noecho(); //don't echo input chars
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE); //don't echo arrow keys
}

void drawInitialScreen(){
    addstr("Hello, World!\nq:quit");
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


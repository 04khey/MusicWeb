#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h> 
#include <getopt.h> 
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "utilityfuncs.h"

typedef struct LIBRARY LIBRARY;
typedef struct MUS_NODE MUS_NODE;



int doSearchWinIteration(WINDOW* searchBar, WINDOW* searchResults, char* searchBuffer, int* highlightLine, LIBRARY* lib, MUS_NODE** outNode){

    
        int input; //= malloc(sizeof(int));
        char inAsChar;

        char* truncatedFileName_temp;
        int maxWidth;

        maxWidth = getmaxx(searchResults);

        truncatedFileName_temp = malloc(maxWidth+1*sizeof(char));

        


        LIBRARY* results = searchLibrary(lib, searchBuffer);

        for(int i=0;i<results->NumNodes;i++){
            strncpy(truncatedFileName_temp, results->Songs[i]->NiceName, maxWidth); //iterate this over songs[i][offset] in a thread and passively redraw?
            if(i==*highlightLine){
                wattron(searchResults,COLOR_PAIR(1));
            }
            mvwprintw(searchResults,i,0, "%s",truncatedFileName_temp); 
            if(i==*highlightLine){
                wattroff(searchResults,COLOR_PAIR(1));
            }
        }
        wrefresh(searchResults);
        //put this here because otherwise the above for loop writes over it if searchBar is below searchResults
        mvwprintw(searchBar, 0, 0, "Search: %d %s",*highlightLine, searchBuffer);
        wrefresh(searchBar);

        input = wgetch(searchBar);
        mvwprintw(searchBar, 0, 0, "Search: %d %s          %d",*highlightLine, searchBuffer, input);
        switch (input)
        {
        case KEY_BACKSPACE:
            //mvwprintw(searchResults, 0, 0, "HIT!: %s          ", searchBuffer);
            searchBuffer[strlen(searchBuffer)-1]='\0';
        break;
        case KEY_UP:
        if(*highlightLine==0){

        } else {
            (*highlightLine)--;
        }
        break;
        case KEY_LEFT:
        case KEY_RIGHT:
        break; //otherwise these get ignored
        case KEY_DOWN:
        if(*highlightLine==results->NumNodes-1){

        } else {
            //(*highlightLine) = (*highlightLine)+1;
            (*highlightLine)++;
        }
        break;
        case KEY_ENTER:
        case '\n':
        if(results->NumNodes >0){
            *outNode = results->Songs[*highlightLine];
            return input;
        }
        break;
        default:
        if(input>31 && input < 127){
            inAsChar = (char) input;
            strncat(searchBuffer, &inAsChar, 1);
        } else{
            input = '\0';
        }
        break;
        }
        wclear(searchResults);
        
        results = searchLibrary(lib, searchBuffer);

        if(results->NumNodes<=*highlightLine){
            *highlightLine=0;
        }
        /*
        for(int i=0;i<results->NumNodes;i++){
            strncpy(truncatedFileName_temp, results->Songs[i]->NiceName, maxWidth); //iterate this over songs[i][offset] in a thread and passively redraw?
            if(i==*highlightLine){
                wattron(searchResults,COLOR_PAIR(1));
            }
            mvwprintw(searchResults,i,0, "%s",truncatedFileName_temp); 
            if(i==*highlightLine){
                wattroff(searchResults,COLOR_PAIR(1));
            }
        }
        wrefresh(searchResults);*/
        

        return input;
    
}
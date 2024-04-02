#ifndef musweb_uithing
#define musweb_uithing

#include <ncurses.h>
#include "utilityfuncs.h"

int doSearchWinIteration(WINDOW* searchBar, WINDOW* searchResults, char* searchBuffer, int* highlightLine,struct LIBRARY* lib,struct MUS_NODE** outNode);

#endif
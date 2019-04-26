#ifndef MENULIB_H
#define MENULIB_H

#ifdef USENCURSES
#include <curses.h>
#include <menu.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include "analysislib.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define NUMENTRIES 7

void clearMenu(void);
void createMainMenu(bool color=false);
void createLogFileName(char* progname);
void exitFromMenu(void);
void f12ToExit(void);
void printMessage(const char *routine, const char *message1, const char *message2=0);
void readMenuEntry(void);
void redirectStderr(void);
void redirectStdout(void);
void restoreStderr(void);
void restoreStdout(void);
#ifdef USENCURSES
void clearScreen(WINDOW* win, const int ywin);
WINDOW* getMenuWindow(void);
void menuTitle(WINDOW *win, int starty, int startx, int width, const char *string, bool color=false);
void setMenuFunctions(void);
void refreshMenu(void);
#endif

#endif // MENULIB_H

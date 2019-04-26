#ifndef ANALYSISLIB_H
#define ANALYSISLIB_H

#ifdef USENCURSES
#include <curses.h>
#else
#include <unistd.h>
#include <term.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include "menulib.h"
#include "hiclib.h"

#ifdef USENCURSES
void analyzeAllIBHics(WINDOW *win);
void analyzeAllOBHics(WINDOW *win);
void analyzeAllHICs(const THicType hicType, WINDOW *win);
void analyzeSingleIBHic(WINDOW *win);
void analyzeSingleOBHic(WINDOW *win);
void analyzeSingleHIC(const THicType hicType, WINDOW *win);
void helpUsage(WINDOW *win);
#else
void analyzeAllIBHics(void);
void analyzeAllOBHics(void);
void analyzeAllHICs(const THicType hicType);
void analyzeSingleIBHic(void);
void analyzeSingleOBHic(void);
void analyzeSingleHIC(const THicType hicType);
void helpUsage(void);
#endif
int chooseActivity(const int nact);
int chooseAnalysis(const int nact);
AlpideDB *initAlpideDB(void);

#endif // ANALYSISLIB_H

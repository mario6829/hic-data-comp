#ifndef HICLIB_H
#define HICLIB_H

#include "DBHelpers.h"
#include "AlpideDB.h"
#include "AlpideDBEndPoints.h"
#include "TScanFactory.h"

#ifdef USENCURSES
#include <curses.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>

std::string askHicName(void);
std::string askStaveName(void);

#endif // HICLIB_H

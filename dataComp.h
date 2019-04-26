#ifndef DATACOMP_H
#define DATACOMP_H

#include "menulib.h"

#include <time.h>

const int kVersion = 1;
const int kSubVersion = 0;

void printHelp(void);
void scanArgs(const int argc, char** argv, bool* help, bool* color);

#endif // DATACOMP_H

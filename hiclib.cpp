#include "hiclib.h"

using namespace std;

string askHicName(void)
{
//
// Asks the user for a HIC name
//
// Inputs:
//
// Outputs:
//
// Return:
//            a string containing the HIC name
//
// Created:      20 Sep 2018  Mario Sitta
//

  char line[80];
  
#ifdef USENCURSES
  mvprintw(1, 1, "Enter the HIC name: ");

  echo();
  getstr(line);
  noecho();
#else
  printf("\n Enter the HIC name: ");

  fgets(line, sizeof(line), stdin);
  line[strlen(line) - 1] = '\0'; // Get rid of CR
#endif

  string hicName(line);

  return hicName;

}

string askStaveName(void)
{
//
// Asks the user for a Stave name
//
// Inputs:
//
// Outputs:
//
// Return:
//            a string containing the Stave name
//
// Created:      25 Apr 2019  Mario Sitta
//

  char line[80];
  
#ifdef USENCURSES
  mvprintw(1, 1, "Enter the Stave name: ");

  echo();
  getstr(line);
  noecho();
#else
  printf("\n Enter the Stave name: ");

  fgets(line, sizeof(line), stdin);
  line[strlen(line) - 1] = '\0'; // Get rid of CR
#endif

  string staveName(line);

  return staveName;

}

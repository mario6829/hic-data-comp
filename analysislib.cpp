#include "analysislib.h"
#include "dctrltestlib.h"
#include "digiscanlib.h"
#include "powertestlib.h"
#include "noisescanlib.h"
#include "threscanlib.h"

// List of available analyses
const int numTotalAnal = 5;

const char* availAnal[numTotalAnal] = {
  "Power Test",
  "Digital scan",
  "Threshold scan",
  "Noise scan",
  "DCTRL Test"
};

#ifdef USENCURSES
void analyzeAllIBHics(WINDOW* win)
#else
void analyzeAllIBHics(void)
#endif
{
//
// Driver routine to analyze all OB HICs
//
// Inputs:
//           win : the ncurses window
//
// Outputs:
//
// Return:
//
// Created:      26 Feb 2019  Mario Sitta
//

#ifdef USENCURSES
  analyzeAllHICs(HIC_IB, win);
#else
  analyzeAllHICs(HIC_IB);
#endif
}

#ifdef USENCURSES
void analyzeAllOBHics(WINDOW* win)
#else
void analyzeAllOBHics(void)
#endif
{
//
// Driver routine to analyze all OB HICs
//
// Inputs:
//           win : the ncurses window
//
// Outputs:
//
// Return:
//
// Created:      26 Feb 2019  Mario Sitta
//

#ifdef USENCURSES
  analyzeAllHICs(HIC_OB, win);
#else
  analyzeAllHICs(HIC_OB);
#endif
}

#ifdef USENCURSES
void analyzeAllHICs(const THicType hicType, WINDOW* win)
#else
void analyzeAllHICs(const THicType hicType)
#endif
{
//
// Driver routine to analyze all IB or OB HICs
//
// Inputs:
//           hicType : the HIC type (IB or OB)
//           win     : the ncurses window
//
// Outputs:
//
// Return:
//
// Created:      02 Oct 2018  Mario Sitta
// Updated:      09 Jan 2019  Mario Sitta
// Updated:      16 Feb 2019  Mario Sitta   Made generic
//

#ifdef USENCURSES
  clearScreen(win, 20);
  WINDOW *analwin = newwin(23,78,1,1);
  wrefresh(analwin);
#else
  system("clear");
  printf("\n\n");
#endif

  // Initialize the DB connection
  // (no need to check if db is valid: if initAlpideDB fails we exit there)
  AlpideDB *db = initAlpideDB();

  // Get the component type and Id
  int componentTypeId;
  if (hicType == HIC_IB)
    componentTypeId = DbGetComponentTypeId (db, "Inner Barrel HIC Module");
  else
    componentTypeId = DbGetComponentTypeId (db, "Outer Barrel HIC Module");

  // Get the list of all available HICs
  ComponentDB *componentDB = new ComponentDB(db);
  std::vector<ComponentDB::componentShort> componentList;  

  componentDB->GetListByType(db->GetProjectId(), componentTypeId, &componentList);
//  mvprintw(3, 2, "Found %d OB HICs - Please wait while analysing them all\n",
//           componentList.size());
  if (hicType == HIC_IB)
    cout << "Found " << componentList.size() << " IB HIC" << endl;
  else
    cout << "Found " << componentList.size() << " OB HIC" << endl;    

  // Ask the user which analysis to perform
  int numAna = chooseAnalysis(1);

  if(numAna != 0) cout << "Please wait while analysing all HICs" << endl;

  switch (numAna) {
    case 0:
    default:  // To refrain gcc from complaining...
#ifdef USENCURSES
      endwin();
#endif
      exit(0);
      break;
    case 1:
      analyzeAllPowerTests(componentList, db, hicType);
      break;
    case 2:
      analyzeAllDigitalScans(componentList, db, hicType);
      break;
    case 3:
      analyzeAllThresholdScans(componentList, db, hicType);
      break;
    case 4:
      analyzeAllNoiseScans(componentList, db, hicType);
      break;
    case 5:
      analyzeAllDCTRLTests(componentList, db, hicType);
//      cout << "Sorry, not yet implemented " << endl; // !!TEMPORARY!!
      break;
  }

//  vector<ComponentDB::compActivity> tests;
//  redirectStdout();
//  for (unsigned int i = 0; i < componentList.size(); i++) {
//    printf("i %d id %d name %s\n",i,componentList.at(i).ID,componentList.at(i).ComponentID.c_str());
//  }
//  restoreStdout();

//  mvprintw(5, 2, "There were %d incomplete activities\n", incomplete);

  f12ToExit();

}

#ifdef USENCURSES
void analyzeSingleIBHic(WINDOW* win)
#else
void analyzeSingleIBHic(void)
#endif
{
//
// Driver routine to analyze a single IB HIC
//
// Inputs:
//           win : the ncurses window
//
// Outputs:
//
// Return:
//
// Created:      26 Feb 2019  Mario Sitta
//

#ifdef USENCURSES
  analyzeSingleHIC(HIC_IB, win);
#else
  analyzeSingleHIC(HIC_IB);
#endif
}

#ifdef USENCURSES
void analyzeSingleOBHic(WINDOW* win)
#else
void analyzeSingleOBHic(void)
#endif
{
//
// Driver routine to analyze a single OB HIC
//
// Inputs:
//           win : the ncurses window
//
// Outputs:
//
// Return:
//
// Created:      26 Feb 2019  Mario Sitta
//

#ifdef USENCURSES
  analyzeSingleHIC(HIC_OB, win);
#else
  analyzeSingleHIC(HIC_OB);
#endif
}

#ifdef USENCURSES
void analyzeSingleHIC(const THicType hicType, WINDOW* win)
#else
void analyzeSingleHIC(const THicType hicType)
#endif
{
//
// Driver routine to analyze a single IB or OB HIC
//
// Inputs:
//           hicType : the HIC type (IB or OB)
//           win     : the ncurses window
//
// Outputs:
//
// Return:
//
// Created:      20 Sep 2018  Mario Sitta
// Updated:      16 Feb 2019  Mario Sitta   Made generic
//

#ifdef USENCURSES
  clearScreen(win, 20);
#else
  system("clear");
  printf("\n");
#endif
  std::string hicName = askHicName();

  // Initialize the DB connection
  // (no need to check if db is valid: if initAlpideDB fails we exit there)
  AlpideDB *db = initAlpideDB();

  // Get the component type and Id
  int componentTypeId;
  if (hicType == HIC_IB)
    componentTypeId = DbGetComponentTypeId (db, "Inner Barrel HIC Module");
  else
    componentTypeId = DbGetComponentTypeId (db, "Outer Barrel HIC Module");

  int componentId   = DbGetComponentId     (db, componentTypeId, hicName);

  if (componentId == -1) {
#ifdef USENCURSES
    mvprintw(3, 1, "HIC %s not found in DataBase! \n", hicName.data());
    f12ToExit();
    endwin();
#else
    printf("\n\n  HIC %s not found in DataBase! \n", hicName.data());
    f12ToExit();
#endif
    exit(0);
  }
    
#ifdef USENCURSES
  mvprintw(3, 1, "HIC %s: componentTypeId %d componentId %d \n",
	   hicName.data(), componentTypeId, componentId);
#else
  printf("\n\n  HIC %s: componentTypeId %d componentId %d \n",
	   hicName.data(), componentTypeId, componentId);
#endif

  // Get the associated activities
  vector<ComponentDB::compActivity> tests;
  redirectStdout();
//  DbGetAllTests (db, componentId, tests, STDigital, false);
  DbGetAllTests (db, componentId, tests, STDigital, true);
  restoreStdout();

  std::vector<ComponentDB::compActivity>::iterator it;
  int nActivity = 0;
  for(it = tests.begin(); it != tests.end(); it++) {
    ComponentDB::compActivity act = *it;
#ifdef USENCURSES
    mvprintw(5+nActivity, 1, "Activity number %2d : %s\n",
#else
    printf(" Activity number %2d : %s\n",
#endif
	     nActivity+1, act.Name.data());
    nActivity++;
  }

#ifdef USENCURSES
  mvprintw(5+nActivity+1, 1, "There are a total of %d activities\n", nActivity);
#else
  printf("\n There are a total of %d activities\n", nActivity);
#endif

  // Ask the user which activity to analyze
  int numAct = chooseActivity(nActivity);

  if (numAct == 0) {
#ifdef USENCURSES
    endwin();
#endif
    exit(0);
  }

  // Ask the user which analysis to perform
  int numAna = chooseAnalysis(nActivity);

  switch (numAna) {
    case 0:
    default:  // To refrain gcc from complaining...
#ifdef USENCURSES
      endwin();
#endif
      exit(0);
      break;
    case 1:
      analyzePowerTest(componentId, tests.at(numAct-1), db, hicType);
      break;
    case 2:
      analyzeDigitalScan(componentId, tests.at(numAct-1), db, hicType);
      break;
    case 3:
      analyzeThresholdScan(componentId, tests.at(numAct-1), db, hicType);
      break;
    case 4:
      analyzeNoiseScan(componentId, tests.at(numAct-1), db, hicType);
      break;
    case 5:
      analyzeDCTRLTest(componentId, tests.at(numAct-1), db, hicType);
      break;
  }

#ifdef USENCURSES
  endwin();
#endif
  exit(0);
}

int chooseActivity(const int nact)
{
//
// Choose which activity to analyze
//
// Inputs:
//            nact : the number of activities available
//
// Outputs:
//
// Return:
//            the id of the choosen activity
//
// Created:      26 Sep 2018  Mario Sitta
// Updated:      26 Feb 2018  Mario Sitta   fflush stdout
//

  char line[80];
  int numAct = -1;

#ifdef USENCURSES
  mvprintw(5+nact+3, 1, "Enter the activity number to be analyzed (0 to exit): ");
  
  echo();
#endif
  while(numAct < 0 || numAct > nact) {
#ifdef USENCURSES
    move(5+nact+3, 55),
    getstr(line);
#else
    printf("\n Enter the activity number to be analyzed (0 to exit): ");
    fflush(stdout);
    fgets(line, sizeof(line), stdin);
#endif
    sscanf(line, "%d", &numAct);
  }
#ifdef USENCURSES
  noecho();
#endif

  return numAct;
}

int chooseAnalysis(const int nact)
{
//
// Choose which analysis to perform
//
// Inputs:
//            nact : the number of activities available
//                   (only used to place correctly the text with ncurses)
//
// Outputs:
//
// Return:
//            the id of the choosen analysis
//
// Created:      26 Sep 2018  Mario Sitta
//

  char line[80];
  int numAna = -1;

#ifndef USENCURSES
  printf("\n");
#endif
  int currpos = nact + 10;
  for (int i = 0; i < numTotalAnal; i++)
#ifdef USENCURSES
    mvprintw(currpos+i, 1, "%d - %s\n", i+1, availAnal[i]);
#else
    printf(" %d - %s\n", i+1, availAnal[i]);
#endif
  currpos += (numTotalAnal + 1);
#ifdef USENCURSES
  mvprintw(currpos, 1, "Enter the analysis number to be performed (0 to exit): ");

  echo();
#else
  printf("\n");
#endif
  while(numAna < 0 || numAna > numTotalAnal) {
#ifdef USENCURSES
    move(currpos, 56),
    getstr(line);
#else
    printf(" Enter the analysis number to be performed (0 to exit): ");
    fgets(line, sizeof(line), stdin);
#endif
    sscanf(line, "%d", &numAna);
  }
#ifdef USENCURSES
  noecho();
#endif

  return numAna;
}

#ifdef USENCURSES
void helpUsage(WINDOW *menu_win)
#else
void helpUsage(void)
#endif
{
//
// Gives summary usage instructions
//
// Inputs:
//            menu_win  : the menu window
//
// Outputs:
//
// Return:
//
// Created:      19 Sep 2018  Mario Sitta
//

  const int numlines = 7;
  std::string helpline[numlines];

  helpline[0] = "This program analyses the data from a given HIC or all HICs,";
  helpline[1] = "performs an analysis of these data, and produces summary";
  helpline[2] = "histograms.";
  helpline[3] = "";
  helpline[4] = "From the main menu the user can choose whether to analyse a";
  helpline[5] = "single HIC, or all HICs from either the Inner or the Outer";
  helpline[6] = "Barrel.";

  // Create the help window with a box around it
#ifdef USENCURSES
  WINDOW *help_win;
  help_win = newwin(18, 70, 2, 4);
  box(help_win, 0, 0);

  for(int i = 0; i < numlines; i++)
    mvwprintw(help_win, 1+i, 1, "%s", helpline[i].data());
  wrefresh(help_win);

  f12ToExit();

  clearScreen(help_win,20);
  
  wclear(help_win);
  delwin(help_win);
  
  box(menu_win, 0, 0);
  wrefresh(menu_win);
#else
  system("clear");

  printf("\n\n\n");
  for(int i = 0; i < numlines; i++)
    printf("  %s\n", helpline[i].data());

  for(int i = 0; i < 20-numlines; i++)
    printf("\n");

  f12ToExit();
#endif
}

AlpideDB *initAlpideDB(void)
{
//
// Initializes the Alpide DB connection
//
// Inputs:
//
// Outputs:
//
// Return:
//            if successful, a pointer to the opened DB
//
// Created:      02 Oct 2018  Mario Sitta
//

  // Initialize the DB connection
  AlpideDB *db = new AlpideDB(false);
  if(!db) {
#ifdef USENCURSES
    mvprintw(3, 1, "Error opening the DB!\n");
    f12ToExit();
    endwin();
#else
    printf("\n\n\n Error opening the DB!\n");
    f12ToExit();
#endif
    exit(0);
  }

  return db;
}

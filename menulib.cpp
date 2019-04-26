#include "menulib.h"

#ifdef USENCURSES
ITEM **my_items;
WINDOW *my_menu_win;
MENU *my_menu;
#endif
int menuSize;

string versubver;

fpos_t posStdout, posStderr;
int fdStdout, fdStderr;

char* logfilename;

const char* menuEntries[NUMENTRIES+1] = {
  "Read data for single IB HIC",
  "Read data for single OB HIC",
  "Read data for all IB HICs",
  "Read data for all OB HICs",
  "Help",
  "Exit"
};

void createLogFileName(char* progname)
{
//
// Creates the log file name from current date and time
//
// Inputs:
//            progname  : the name of the program
//
// Outputs:
//
// Return:
//
// Created:      20 Sep 2018  Mario Sitta
// Updated:      26 Apr 2019  Mario Sitta  Print version number
//

  const int pgname_len = strlen(progname);
  logfilename = (char*)malloc(pgname_len+20);
  if (!logfilename) {
    cerr << "Error allocating space for log file name!" << endl;
    return;
  }

  // Get current time and assmble log file name
  time_t curtim = time(NULL);
  struct tm timst = *localtime(&curtim);

  sprintf(logfilename,"%s_%02d%02d%02d_%02d%02d%02d.log", progname,
          timst.tm_year-100, timst.tm_mon+1, timst.tm_mday,
          timst.tm_hour, timst.tm_min, timst.tm_sec);

  FILE* logfile = fopen(logfilename, "a");
  fprintf(logfile, "dataComp Version %s\n", versubver.c_str());
  fclose(logfile);
}

void createMainMenu(const bool color)
{
//
// Creates the initial menu
//
// Inputs:
//            color : flag to use colored menu entries
//
// Outputs:
//
// Return:
//            the pointer to the menu
//
// Created:      19 Sep 2018  Mario Sitta
// Updated:      25 Oct 2018  Mario Sitta  Add plain menu
// Updated:      26 Apr 2019  Mario Sitta  Print version number
//
#ifdef USENCURSES
  MENU *my_menu;

  // Initialize curses
  initscr();
  if (color)
    start_color();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  init_pair(1, COLOR_RED, COLOR_BLACK);

  // Create items
  menuSize = ARRAY_SIZE(menuEntries);
  my_items = (ITEM **)calloc(menuSize, sizeof(ITEM *));
  for(int i = 0; i < menuSize; ++i)
    my_items[i] = new_item(menuEntries[i], "");
  setMenuFunctions();

  // Crate menu
  my_menu = new_menu((ITEM **)my_items);

  // Create the window to be associated with the menu
  my_menu_win = newwin(10, 40, 4, 20);
  keypad(my_menu_win, TRUE);

  // Set main window and sub window
  set_menu_win(my_menu, my_menu_win);
  set_menu_sub(my_menu, derwin(my_menu_win, 6, 36, 3, 4));

  // Set menu mark to the string " * "
  set_menu_mark(my_menu, " * ");

  // Print a border around the main window and print a title
  box(my_menu_win, 0, 0);
  menuTitle(my_menu_win, 1, 0, 40, "HIC Data Analysis", color);
  mvwaddch(my_menu_win, 2, 0, ACS_LTEE);
  mvwhline(my_menu_win, 2, 1, ACS_HLINE, 38);
  mvwaddch(my_menu_win, 2, 39, ACS_RTEE);
//  mvprintw(LINES - 2, 0, "F12 to exit");
  refresh();

  post_menu(my_menu);
#else
  system("clear");
  int j;
  printf("\n\n\n\n\n\n\n");
  printf("\t\t\tHIC Data Analysis\n");
  printf("\t\t\t    Version %s\n\n", versubver.c_str());
  for (j=0; j<NUMENTRIES; j++)
    printf("\t\t%2d - %s\n",j+1,menuEntries[j]);
#endif
  return;
}

void clearMenu(void)
{
//
// Clears the initial menu
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      19 Sep 2018  Mario Sitta
// Updated:      25 Oct 2018  Mario Sitta
//
#ifdef USENCURSES
  // Clean up and exit
  unpost_menu(my_menu);

  for(int i = 0; i < menuSize; ++i)
    free_item(my_items[i]);
  free_menu(my_menu);

  endwin();
#endif
}

#ifdef USENCURSES
void clearScreen(WINDOW* win, const int ywin)
{
//
// Clears the screen (a bit cumbersome, since (n)curses does not
// have an effective routine to do the task)
//
// Inputs:
//            win : the current window
//            ywin : the window vertical dimension
//
// Outputs:
//
// Return:
//
// Created:      20 Sep 2018  Mario Sitta
//

  char blnkstr[80] = "                                                                               ";

  if (ywin <= 0) return; 

  for(int i = 0; i <= ywin; i++)
    mvwprintw(win, i, 0, "%s", blnkstr);
  wrefresh(win);

  mvprintw(LINES - 2, 0, "           ");

}
#endif

void exitFromMenu(void)
{
//
// Exits the program from the menu entry
//
// Inputs:
//            my_menu : the menu to be cleared
//
// Outputs:
//
// Return:
//
// Created:      19 Sep 2018  Mario Sitta
// Updated:      25 Oct 2018  Mario Sitta
//

  clearMenu();

  exit(0);
}

void f12ToExit(void)
{
//
// Waits for F12 to be pressed
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      26 Sep 2018  Mario Sitta
//
#ifdef USENCURSES
  mvprintw(LINES - 2, 0, "F12 to exit");
  while(getch() != KEY_F(12));
#else
  printf("\nPress any key to exit");
  getchar();
  system("clear");
#endif
}

#ifdef USENCURSES
WINDOW* getMenuWindow(void)
{
//
// Returns the pointer to the menu window
//
// Inputs:
//
// Outputs:
//
// Return:
//            the pointer to the menu window
//
// Created:      20 Sep 2018  Mario Sitta
//

  return my_menu_win;
}

void menuTitle(WINDOW *win, int starty, int startx, int width, const char *string, bool color)
//
// Puts a title in the middle of the menu window
//
// Inputs:
//            win     : the window where to put the title
//            startx,starty : the initial coordinates where to put the title
//            width   : the title width
//            string  : the title string
//            color   : whether to use colors
//
// Outputs:
//
// Return:
//
// Created:      19 Sep 2018  Mario Sitta
//
{
  int length, x, y;
  float temp;

  if(win == NULL)
    win = stdscr;
  getyx(win, y, x);
  if(startx != 0)
    x = startx;
  if(starty != 0)
    y = starty;
  if(width == 0)
    width = 80;

  length = strlen(string);
  temp = (width - length)/ 2;
  x = startx + (int)temp;

  if (color)
    wattron(win, color);
  mvwprintw(win, y, x, "%s", string);
  if (color)
    wattroff(win, color);

  refresh();
}
#endif

void printMessage(const char *routine, const char *message1, const char *message2)
{
//
// Prints an error message
//
// Inputs:
//            routine  : the name of the function generating the message
//            message1 : the actual message (mandatory)
//            message2 : an optional second message
//
// Outputs:
//
// Return:
//
// Created:      27 Sep 2018  Mario Sitta
// Updated:      25 Oct 2018  Mario Sitta
//
 
#ifdef USENCURSES
  if (message2 == 0)
    printw("%s: %s\n", routine, message1);
  else
    printw("%s: %s %s\n", routine, message1, message2);
#else
  if (message2 == 0)
    printf("%s: %s\n", routine, message1);
  else
    printf("%s: %s %s\n", routine, message1, message2);
#endif

}

void readMenuEntry(void)
{
//
// Reads user's menu entry and exectutes the corresponding action
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      25 Oct 2018  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta
//

  int c;

#ifdef USENCURSES
  while((c = getch()) != KEY_F(12)){
    switch(c)
      {
        case KEY_DOWN:
          menu_driver(my_menu, REQ_DOWN_ITEM);
          break;
        case KEY_UP:
          menu_driver(my_menu, REQ_UP_ITEM);
          break;
        case 10: /* Enter */
          {
//            void (*p)(char *);
            void (*p)(void*);
	    WINDOW *my_menu_win = getMenuWindow();
            ITEM *cur = current_item(my_menu);
//            p = (void(*)(char*))item_userptr(cur);
//            p((char *)item_name(cur));
            p = (void(*)(void*))item_userptr(cur);
            int curidx = item_index(cur);
            switch(curidx)
              {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
		  p((void*)my_menu_win);
                  break;
                case 5:
                  p((void*)my_menu);
                  break;
              }
            pos_menu_cursor(my_menu);
            break;
          }
          break;
      }
    refreshMenu();
  }
#else
  char line[80];
  printf("\n");
  do {
    printf(" Enter choice: ");
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d", &c);
    switch(c)
      {
        case 1:
	  analyzeSingleIBHic();
	  c = 6; // Exit
	  break;
        case 2:
	  analyzeSingleOBHic();
	  c = 6; // Exit
	  break;
        case 3:
	  analyzeAllIBHics();
	  c = 6; // Exit
	  break;
        case 4:
	  analyzeAllOBHics();
	  c = 6; // Exit
	  break;
        case 5:
	  helpUsage();
	  createMainMenu(0);
	  break;
        case 6:
	  break;
      }
  } while (c != 6);
#endif
}
  
void redirectStderr(void)
{
//
// Redirects stderr to file to avoid mangling the menu screen
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      20 Sep 2018  Mario Sitta
//

  // Redirect to file but keep memory of current stdout position
  fflush(stderr);
  fgetpos(stderr, &posStderr);
  fdStderr = dup(fileno(stderr));
  freopen(logfilename, "a", stderr);

}

void redirectStdout(void)
{
//
// Redirects stdout to file to avoid mangling the menu screen
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      20 Sep 2018  Mario Sitta
//

  // Redirect to file but keep memory of current stdout position
  fflush(stdout);
  fgetpos(stdout, &posStdout);
  fdStdout = dup(fileno(stdout));
  freopen(logfilename, "a", stdout);

}

#ifdef USENCURSES
void refreshMenu(void)
{
//
// Refreshes the menu
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      19 Sep 2018  Mario Sitta
//

  wrefresh(my_menu_win);
}
#endif

void restoreStderr(void)
{
//
// Restores stdout to screen after redirection to file
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      20 Sep 2018  Mario Sitta
//

  // Flush file and restore stdout
  fflush(stderr);
  dup2(fdStderr, fileno(stderr));
  close(fdStderr);
  clearerr(stderr);
  fsetpos(stderr, &posStderr);

}

void restoreStdout(void)
{
//
// Restores stdout to screen after redirection to file
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      20 Sep 2018  Mario Sitta
//

  // Flush file and restore stdout
  fflush(stdout);
  dup2(fdStdout, fileno(stdout));
  close(fdStdout);
  clearerr(stdout);
  fsetpos(stdout, &posStdout);

}

#ifdef USENCURSES
void setMenuFunctions(void)
{
//
// Assigns functions to menu entries
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      19 Sep 2018  Mario Sitta
//

  set_item_userptr(my_items[0], (void*)analyzeSingleIBHic);
  set_item_userptr(my_items[1], (void*)analyzeSingleOBHic);
  set_item_userptr(my_items[2], (void*)analyzeAllIBHics);
  set_item_userptr(my_items[3], (void*)analyzeAllOBHics);
  set_item_userptr(my_items[4], (void*)helpUsage);
  set_item_userptr(my_items[5], (void*)exitFromMenu);

}
#endif

void setVersionNumber(const int version, const int subversion)
{
//
// Saves the version number into a (global) string
//
// Inputs:
//            version    : the version number
//            subversion : the subversion number
//
// Outputs:
//
// Return:
//
// Created:      26 Apr 2019  Mario Sitta
//

  versubver = to_string(version) + "." + to_string(subversion);

}

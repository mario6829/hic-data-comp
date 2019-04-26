#include "dataComp.h"

using namespace std;

void printHelp(void)
{
//
// Prints a usage message
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      19 Sep 2018  Mario Sitta
//

  cout << endl << "Usage:" << endl;
  cout << "   dataComp [-h|--help] [-c|--color]" << endl;
  cout << "             -h|--help   prints this message" << endl;
  cout << "             -c|--color  uses colored menus" << endl;
}

void scanArgs(const int argc, char** argv, bool* help, bool* color)
{
//
// Scans the argument vector
//
// Inputs:
//            argc  : the number of arguments (from main)
//            argv  : the argument vector (from main)
//            help  : the help flag
//            color : the color flag
//
// Outputs:
//            help  : the help flag
//            color : the color flag
//
// Return:
//
// Created:      19 Sep 2018  Mario Sitta
//

  if (argc == 1) return;  // User passed no arguments

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if ((arg == "-h") || (arg == "--help"))
      *help = true;
    if ((arg == "-c") || (arg == "--color"))
      *color = true;
  }

}

int main(int argc, char** argv)
{
  bool help=false, color=false;

  scanArgs(argc, argv, &help, &color);

  if (help) {
    printHelp();
    exit(0);
  }

  createLogFileName(argv[0]);
  redirectStderr();

  createMainMenu(color);

  // Post the menu
//  post_menu(my_menu);
#ifdef USENCURSES
  refreshMenu();
#endif

  readMenuEntry();
  clearMenu();

  return 0;
}


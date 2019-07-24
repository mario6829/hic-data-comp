#include "utillib.h"
#include "menulib.h"

Int_t AskUserRedoScan(void)
{
//
// Ask the user whether to redo the scan from beginning or re-use
// the existing ROOT file
//
// Inputs:
//
// Outputs:
//
// Return:
//          the chosen option
//
// Created:      17 Jan 2019  Mario Sitta
//

#ifdef USENCURSES
  mvprintw(3, 1, "Root file already exists. Do you want to: ");
  mvprintw(5, 1, "1 - Re-analyze all HICs/Activities");
  mvprintw(6, 1, "2 - Add missing HICs/Activities to present file");
#else
  printf("\n\n Root file already exists. Do you want to: ");
  printf("\n 1 - Re-analyze all HICs/Activities\n");
  printf(" 2 - Add missing HICs/Activities to present file\n");
#endif

  Char_t line[80];
  Int_t choice = 0;
  while(choice < 1 || choice > 2) {
#ifdef USENCURSES
    echo();
    getstr(line);
    noecho();
#else
    fgets(line, sizeof(line), stdin);
    line[strlen(line) - 1] = '\0'; // Get rid of CR
#endif
    sscanf(line, "%d", &choice);
  }

  return choice;
}

Bool_t CheckRootFileExists(TString name)
{
//
// Checks if the Root file already exists
//
// Inputs:
//          name  : the file name
//
// Outputs:
//
// Return:
//          true if file exists
//
// Created:      27 Nov 2018  Mario Sitta
//

  struct stat buffer;
  if (stat(name.Data(), &buffer) == 0)
    return kTRUE;
  else
    return kFALSE;

}

Int_t ChipPositionMAM2Test(const THicType hicType, const string position)
{
//
// Converts the chip position from MAM numbering to Test numbering
//
// MAM convention:
//    for IBHIC: HA0 -> HA8
//    for OBHIC: HA1 -> HA7 for the upper row, HB1 -> HB7 for the lower row,
//               the two masters are HA7 and HB1
// Test convention:
//    for IBHIC: 0 -> 8
//    for OBHIC: 0 -> 7 for the upper row, 8 -> 14 for the lower row,
//               the two masters are 0 and 8
//
// Inputs:
//          hicType  : the HIC type (IB or OB)
//          position : the chip position in MAM convention
//
// Outputs:
//
// Return:
//          the chip position in Test convention (-1 in case of error)
//
// Created:      05 Jul 2019  Mario Sitta
//

  Int_t posTest = -1;

  if(position.length() != 3) {
    printMessage("\nChipPositionMAM2Test", "Wrong chip position (MAM) ", position.c_str());
    return posTest;
  }

  switch(hicType) {
    case HIC_IB:
      if(position.substr(0,2) != "HA")
	printMessage("\nChipPositionMAM2Test", "Wrong IB chip position (MAM) ", position.c_str());
      else
	posTest = atoi(position.substr(2,1).c_str()); // Should test if 9...
      break;
    case HIC_OB:
      if(position.substr(0,2) != "HA" && position.substr(0,2) != "HB")
	printMessage("\nChipPositionMAM2Test", "Wrong OB chip position (MAM) ", position.c_str());
      else {
	Int_t pos = atoi(position.substr(2,1).c_str());
	if(pos == 0 && pos > 7)
	  printMessage("\nChipPositionMAM2Test", "Wrong OB chip position (MAM) ", position.c_str());
	else {
	  if(position.substr(0,2) == "HA")
	    posTest = 7 - pos;
	  else
	    posTest = 7 + pos;
	}
      }
      break;
    default:
      printMessage("\nChipPositionMAM2Test", "Wrong HIC type ", to_string(hicType).c_str());
      break;
  }

  return posTest;
}

string ChipPositionTest2MAM(const THicType hicType, const Int_t position)
{
//
// Converts the chip position from Test numbering to MAM numbering
//
// MAM convention:
//    for IBHIC: HA0 -> HA8
//    for OBHIC: HA1 -> HA7 for the upper row, HB1 -> HB7 for the lower row,
//               the two masters are HA7 and HB1
// Test convention:
//    for IBHIC: 0 -> 8
//    for OBHIC: 0 -> 7 for the upper row, 8 -> 14 for the lower row,
//               the two masters are 0 and 8
//
// Inputs:
//          hicType  : the HIC type (IB or OB)
//          position : the chip position in Test convention
//
// Outputs:
//
// Return:
//          the chip position in MAM convention (empty string in case of error)
//
// Created:      05 Jul 2019  Mario Sitta
//

  string posMAM = "";

  if(position < 0) {
    printMessage("\nChipPositionMAM2Test", "Wrong chip position (Test) ", to_string(position).c_str());
    return posMAM;
  }

  switch(hicType) {
    case HIC_IB:
      if(position > 8)
	printMessage("\nChipPositionMAM2Test", "Wrong IB chip position (Test) ", to_string(position).c_str());
      else
	posMAM = "HA" + position;
      break;
    case HIC_OB:
      if(position == 7 || position > 14)
	printMessage("\nChipPositionMAM2Test", "Wrong IB chip position (Test) ", to_string(position).c_str());
      else {
	if(position < 7)
	  posMAM = "HA" + (7 - position);
	else
	  posMAM = "HB" + (position - 7);
      }
      break;
    default:
      printMessage("\nChipPositionMAM2Test", "Wrong HIC type ", to_string(hicType).c_str());
      break;
  }

  return posMAM;
}

void CloseRootFile(TFile *rootfile)
{
//
// Saves the tree
//
// Inputs:
//          rootfile : the pointer to the ROOT file
//
// Outputs:
//
// Return:
//
// Created:      18 Sep 2018  Mario Sitta
// Updated:      08 Oct 2018  Mario Sitta
//

  rootfile->Close();

}

Char_t ConvertTestResult(const string result)
{
//
// Converts the test result into a number to be saved in the trees
//
// Inputs:
//          result : the test result
//
// Outputs:
//
// Return:
//          the test result as a char integer
//
// Created:      28 Jun 2019  Mario Sitta
//

  if(result.find("GOLD_NOBB") != string::npos)
    return CLASS_GOLD_NOBB;
  if(result.find("SILVER_NOBB") != string::npos)
    return CLASS_SILVER_NOBB;
  if(result.find("BRONZE_NOBB") != string::npos)
    return CLASS_BRONZE_NOBB;
  if(result.find("GOLD") != string::npos)
    return CLASS_GOLD;
  if(result.find("SILVER") != string::npos)
    return CLASS_SILVER;
  if(result.find("BRONZE") != string::npos)
    return CLASS_BRONZE;
  if(result.find("PARTIALB") != string::npos)
    return CLASS_PARTIALB;
  if(result.find("PARTIAL") != string::npos)
    return CLASS_PARTIAL;
  if(result.find("NOBBB") != string::npos)
    return CLASS_NOBBB;
  if(result.find("NOBB") != string::npos)
    return CLASS_NOBB;

  return 0;
}

string FindEOSPath(ActivityDB::activityLong actlong, const THicType hicType)
{
//
// Finds the correct EOS path for the given activity
//
// Inputs:
//          actlong : the activityLong for which the path has to be found
//          hicType : the HIC type (IB or OB)
//
// Outputs:
//
// Return:
//          the EOS path as a string
//
// Created:      09 Oct 2018  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
//

  FixActName(actlong, hicType);

  string eosPathSingle = GetEosPath(actlong, hicType, false);
  string eosPathDouble = GetEosPath(actlong, hicType, true);

  string eosPath = "";
  struct stat eosStat;
  if (!(stat(eosPathSingle.data(), &eosStat)))
    eosPath = eosPathSingle;
  if (!(stat(eosPathDouble.data(), &eosStat)))
    eosPath = eosPathDouble;

  return eosPath;
}

void FixActName(ActivityDB::activityLong &actlong, const THicType hicType)
{
//
// Fixes the name of the activity in some pathological cases
//
// Inputs:
//          actlong : the activityLong whose name has to be fixed
//
// Outputs:
//          actlong : the same activityLong with the name fixed
//
// Return:
//
// Created:      10 Oct 2018  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  Check also IB HIC names
//

  if (hicType == HIC_IB) {
    if(actlong.Name.find("IBHIC") != string::npos)
      return;

    // In some cases it is written "IB-HIC"
    string hicDash = "IB-HIC";
    size_t hicDashPos = actlong.Name.find(hicDash);
    if(hicDashPos != string::npos)
      actlong.Name.replace(hicDashPos,hicDash.length(),"IBHIC");

    // In at least one case it is misspelled "IBIHC"
    string hicMisp = "IBIHC";
    size_t hicMispPos = actlong.Name.find(hicMisp);
    if(hicMispPos != string::npos)
      actlong.Name.replace(hicMispPos,hicMisp.length(),"IBHIC");

  } else { // OB HICs

    // Some are missing the "OBHIC" string
    if(actlong.Name.find("OBHIC") != string::npos)
      return;

    // In one case "hic" is lowercase
    string hicLower = "hic";
    size_t hicLowPos = actlong.Name.find(hicLower);
    if(hicLowPos != string::npos)
      actlong.Name.replace(hicLowPos,hicLower.length(),"HIC");

    // In other cases the string is missing
    size_t namePos = -1;
    if(actlong.Name.find("AL") != string::npos)
      namePos = actlong.Name.find("AL");
    if(actlong.Name.find("AR") != string::npos)
      namePos = actlong.Name.find("AR");
    if(actlong.Name.find("BL") != string::npos)
      namePos = actlong.Name.find("BL");
    if(actlong.Name.find("BR") != string::npos)
      namePos = actlong.Name.find("BR");

    if (namePos == -1) return; // Should never happen, leave Name as it is

    actlong.Name.insert(namePos, "OBHIC-");
  }
}

TFile* OpenRootFile(TString name, Bool_t recreate)
{
//
// Opens the ROOT file
//
// Inputs:
//          name  : the file name
//          recreate  : if true, open file in RECREATE mode
//
// Outputs:
//
// Return:
//          a pointer to the created ROOT file
//
// Created:      18 Sep 2018  Mario Sitta
// Updated:      08 Oct 2018  Mario Sitta
// Updated:      27 Nov 2018  Mario Sitta/
// Updated:      17 Jan 2019  Mario Sitta
//

  TFile *rootfile = 0;

  if (recreate)
    rootfile = new TFile(name.Data(),"RECREATE");
  else
    rootfile = new TFile(name.Data());

  return rootfile;
}

Bool_t RenameExistingRootFile(TString oldname, TString mod, TString &newname)
{
//
// Rename the existing ROOT file
// WARNING: for sake of simplicity, we assume the file name is always
// in the form whateverstring.root (i.e. it terminates with ".root")
//
// Inputs:
//          oldname : the old file name
//          mod     : the string to be appended to the old name
//
// Outputs:
//          newname : the new file name
//
// Return:
//          kTRUE if rename successful, otherwise kFALSE
//
// Created:      17 Jan 2019  Mario Sitta
//

  TString cmd;

  newname = "";

  Ssiz_t lastDot = oldname.Last('.');
  if(lastDot == kNPOS) return kFALSE; // Should never happen given the file name

//  newname = oldname(0,lastDot-1) + mod + ".root";
  newname = oldname(0,lastDot) + mod + ".root";

  cmd.Form("mv -f %s %s",oldname.Data(),newname.Data());
  if(gSystem->Exec(cmd.Data()) != 0) {
    newname = "";
    return kFALSE;
  } else
    return kTRUE;
}

TFile* SetupRootFile(TString filename, Bool_t &redo)
{
//
// Checks whether the Root file already exists: if not, create a new one,
// if yes, try to open it
//
// Inputs:
//          filename : the file name
//
// Outputs:
//          redo : true if user chose to redo the analysis
//
// Return:
//          a pointer to the created ROOT file
//
// Created:      28 Nov 2018  Mario Sitta
// Updated:      09 Jan 2019  Mario Sitta
// Updated:      17 Jan 2019  Mario Sitta
//

  TFile *newfile = 0;

  redo = kTRUE;

  if (CheckRootFileExists(filename))
    if(AskUserRedoScan() == 2) redo = kFALSE;

  if(redo)
    newfile = OpenRootFile(filename, kTRUE);
  else
    newfile = OpenRootFile(filename, kFALSE);

  return newfile;
}

void WaferNumAndPos(const THicType hicType, std::vector<TChild> children, const Int_t chipNum, Char_t &waferNum, Char_t &waferPos)
{
//
// Returns the wafer number and chip position in the wafer for a given chip
//
// Inputs:
//          hicType : the HIC type (IB or OB)
//          children : vector of all HIC children
//          chipNum : the chip number in the HIC (Test convention)
//
// Outputs:
//          waferNum : the wafer number
//          waferPos : the chip position in the wafer
//
// Return:
//
// Created:      05 Jul 2019  Mario Sitta
//

  Int_t faund;

  waferNum = -1;
  waferPos = -1;

  if (children.size() > 0) {
    std::vector<TChild>::iterator ich;
    for(ich = children.begin(); ich != children.end(); ich++) {
      TChild child = *ich;

      if (ChipPositionMAM2Test(hicType, child.Position) == chipNum) {
        sscanf(child.Name.c_str(), "T%dW%02dR%02d", &faund, &waferNum, &waferPos);
        break;
      }
    }
  }

}


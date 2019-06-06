#include "powertestlib.h"
#include "utillib.h"
#include "menulib.h"
#include "treevariables.h"

#define NUMIVPOINTS 41

// Local tree variables
Float_t  ivCurrent[NUMIVPOINTS];
Float_t  idddAtSwitchOn;
Float_t  iddaAtSwitchOn;
Float_t  idddWithClock;
Float_t  iddaWithClock;
Float_t  idddConfigured;
Float_t  iddaConfigured;
Float_t  ibias0V;
Float_t  ibias3V;
Float_t  classificPowTest;



void analyzeAllPowerTests(std::vector<ComponentDB::componentShort> componentList, AlpideDB *db, const THicType hicType)
{
//
// Steering routine to analyze the data of Power Test for all HICs
//
// Inputs:
//          componentList : a vector of componentShort with the list
//                          of all HICs
//          db : a pointer to the Alpide DB
//          hicType : the HIC type (IB or OB)
//
// Outputs:
//
// Return:
//
// Created:      09 Jan 2019  Mario Sitta
// Updated:      25 Jan 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
// Updated:      08 Mar 2019  Mario Sitta  HIC position added
//                                         Flag ML/OL staves
//                                         Stave Reception Test added
//

  // We need to define here the TTree's for the existing ROOT file
  TTree *oldHicQualTree = 0, *oldHicRecpTree = 0, *oldHicHSTree = 0, *oldHicStaveQualTree = 0, *oldHicStaveRecpTree = 0;;
  TTree *oldHicQualResTree = 0, *oldHicRecpResTree = 0, *oldHicHSResTree = 0, *oldHicStaveQualResTree = 0, *oldHicStaveRecpResTree = 0;
  TTree *oldActFastListTree = 0;

  // Should never happen (the caller should have created it for us)
  if (!db) {
    printMessage("analyzeAllPowerTests","Error: the DataBase was not opened");
    f12ToExit();
    return;
  }

  ActivityDB *activityDB = new ActivityDB(db);

  // Check whether the ROOT file already exists
  // If yes, ask the user whether to use it or redo a new one
  TString rootFileName;
  if (hicType == HIC_IB)
    rootFileName = "IBHIC_PowerTest_AllHICs.root";
  else
    rootFileName = "OBHIC_PowerTest_AllHICs.root";

  redoFromStart = kTRUE;
  if(CheckRootFileExists(rootFileName)) {
    if(AskUserRedoScan() == 2) { // User chose to re-use existing tree
      redoFromStart = kFALSE;
      TString oldRootFileName;
      if(!RenameExistingRootFile(rootFileName, "_old", oldRootFileName)) {
        printMessage("\nanalyzeAllPowerTests","Error: error renaming existing ROOT file");
        f12ToExit();
        return;
      }

      TFile *oldPowertestFile = OpenRootFile(oldRootFileName);
      if(!oldPowertestFile) {
        printMessage("\nanalyzeAllPowerTests","Error: error opening existing ROOT file");
        f12ToExit();
        return;
      }

      oldHicQualTree = ReadPowTestTree("hicQualTree",oldPowertestFile);
      oldHicRecpTree = ReadPowTestTree("hicRecpTree",oldPowertestFile);
      oldHicHSTree = ReadPowTestTree("hicHSTree",oldPowertestFile);
      oldHicStaveQualTree = ReadPowTestTree("hicStaveQualTree",oldPowertestFile);
      oldHicStaveRecpTree = ReadPowTestTree("hicStaveRecpTree",oldPowertestFile);

      oldHicQualResTree = ReadPowTestTreeResult("hicQualResTree",oldPowertestFile);
      oldHicRecpResTree = ReadPowTestTreeResult("hicRecpResTree",oldPowertestFile);
      oldHicHSResTree = ReadPowTestTreeResult("hicHSResTree",oldPowertestFile);
      oldHicStaveQualResTree = ReadPowTestTreeResult("hicStaveQualResTree",oldPowertestFile);
      oldHicStaveRecpResTree = ReadPowTestTreeResult("hicStaveRecpResTree",oldPowertestFile);

      oldActFastListTree = ReadHicActListTreePT(oldPowertestFile);

      if(!oldHicQualTree || !oldHicRecpTree || !oldHicHSTree || !oldHicStaveQualTree || !oldHicStaveRecpTree ||
         !oldHicQualResTree || !oldHicRecpResTree || !oldHicHSResTree || !oldHicStaveQualResTree || !oldHicStaveRecpResTree ||
         !oldActFastListTree) {
        printMessage("\nanalyzeAllPowerTests","Error: error reading trees from existing ROOT file");
        f12ToExit();
        return;
      }
    } // if(AskUserRedoScan())
  } // if(CheckRootFileExists())

  // Open the ROOT file
  TFile *newPowtestFile = OpenRootFile(rootFileName, kTRUE);

  if(!newPowtestFile) {
    printMessage("\nanalyzeAllPowerTests","Error: error opening new ROOT file");
    f12ToExit();
    return;
  }

  // Create or read the trees
  TTree *hicQualTree = CreateTreePowerTest("hicQualTree","HicQualificationTest");
  TTree *hicRecpTree = CreateTreePowerTest("hicRecpTree","HicReceptionTest");
  TTree *hicHSTree = CreateTreePowerTest("hicHSTree","HicHalfStaveTest");
  TTree *hicStaveQualTree = CreateTreePowerTest("hicStaveQualTree","HicStaveQualTest");
  TTree *hicStaveRecpTree = CreateTreePowerTest("hicStaveRecpTree","HicStaveRecpTest");

  TTree *hicQualResTree = CreateTreePowerTestResult("hicQualResTree","HicQualificationTestResults");
  TTree *hicRecpResTree = CreateTreePowerTestResult("hicRecpResTree","HicReceptionTestResults");
  TTree *hicHSResTree = CreateTreePowerTestResult("hicHSResTree","HicHalfStaveTestResults");
  TTree *hicStaveQualResTree = CreateTreePowerTestResult("hicStaveQualResTree","HicStaveQualTestResults");
  TTree *hicStaveRecpResTree = CreateTreePowerTestResult("hicStaveRecpResTree","HicStaveRecpTestResults");

  TTree *actFastListTree = CreateHicActListTreePT();

  // Loop on all components
  int totHICAnal = 0, totActAnal = 0;
  std::vector<ComponentDB::compActivity> tests;
  std::vector<ComponentDB::componentShort>::iterator iComp;
  for (iComp = componentList.begin(); iComp != componentList.end(); iComp++) {
    ComponentDB::componentShort comp = *iComp;
    DbGetAllTests (db, comp.ID, tests, STPower, true);

    // Loop on all activities
    std::vector<ComponentDB::compActivity>::iterator it;
    for(it = tests.begin(); it != tests.end(); it++) {
      ComponentDB::compActivity act = *it;
      ActivityDB::activityLong actLong;
      activityDB->Read(act.ID, &actLong);

      TTree *testree = 0, *resultree = 0;
      TTree *oldtestree = 0, *oldresultree = 0;
      if(actLong.Type.Name.find("HIC") != string::npos &&
	 actLong.Type.Name.find("Qualification") != string::npos) {
        testree = hicQualTree;
	resultree = hicQualResTree;
        oldtestree = oldHicQualTree;
        oldresultree = oldHicQualResTree;
	actMask = ACTMASK_QUALIF;
	staveOLML = 0;
      }
      if(actLong.Type.Name.find("HIC") != string::npos &&
	 actLong.Type.Name.find("Reception") != string::npos) {
        testree = hicRecpTree;
	resultree = hicRecpResTree;
        oldtestree = oldHicRecpTree;
        oldresultree = oldHicRecpResTree;
	actMask = ACTMASK_RECEPT;
	staveOLML = 0;
      }
      if(actLong.Type.Name.find("HS") != string::npos &&
	(actLong.Type.Name.find("ML") != string::npos ||
         actLong.Type.Name.find("OL") != string::npos)) {
        testree = hicHSTree;
	resultree = hicHSResTree;
        oldtestree = oldHicHSTree;
        oldresultree = oldHicHSResTree;
	actMask = ACTMASK_HALFST;
	if(actLong.Type.Name.find("ML") != string::npos)
	  staveOLML = 1;
	else
	  staveOLML = 2;
      }
      if(actLong.Type.Name.find("Stave") != string::npos &&
	(actLong.Type.Name.find("ML") != string::npos ||
         actLong.Type.Name.find("OL") != string::npos)) {
	if(actLong.Type.Name.find("Qualification") != string::npos) {
	  testree = hicStaveQualTree;
	  resultree = hicStaveQualResTree;
	  oldtestree = oldHicStaveQualTree;
	  oldresultree = oldHicStaveQualResTree;
	  actMask = ACTMASK_STAVET;
	}
	if(actLong.Type.Name.find("Reception") != string::npos) {
	  testree = hicStaveRecpTree;
	  resultree = hicStaveRecpResTree;
	  oldtestree = oldHicStaveRecpTree;
	  oldresultree = oldHicStaveRecpResTree;
	  actMask = ACTMASK_STVREC;
	}
	if(actLong.Type.Name.find("ML") != string::npos)
	  staveOLML = 1;
	else
	  staveOLML = 2;
      }

      if (!testree) continue; // Not a Qualification/Reception/HS/Stave test

      if(!redoFromStart)
	if(FindActivityInPowTestTree(oldActFastListTree, comp.ID, act.ID, actMask)) {
          printMessage("\nanalyzeAllPowerTests", "Activity already in file, copying trees ", actLong.Name.c_str());
          CopyPowTestOldToNew(comp.ID, act.ID, testree, resultree, oldtestree, oldresultree);
          actFastListTree->Fill();
          continue;
        }

      string eosPath = FindEOSPath(actLong, hicType);
      if(eosPath.length() == 0) { // No valid path found on EOS
        string hicAct = actLong.Name + " " + actLong.Type.Name;
        printMessage("\nanalyzeAllPowerTests", "EOS for this activity does not exists", hicAct.c_str());
        continue;
      }

      // Fill the tree for all chips
      testOffset = testree->GetEntries();
      testResOffset = resultree->GetEntries();

      hicPosition = DbGetPosition(db, comp.ID);

      PowerTestAllChips(testree, actLong, comp.ID, act.ID, eosPath);
      PowerTestResults(resultree, actLong, comp.ID, act.ID, eosPath, hicType);

      Long64_t prevTestOffset = testree->GetEntries();
      Long64_t prevTestResOffset = resultree->GetEntries();

      if(testOffset == prevTestOffset || testResOffset == prevTestResOffset)
        printMessage("\nanalyzeAllPowerTests", "Trees not filled for activity ", actLong.Name.c_str());
      else
	actFastListTree->Fill();

      totActAnal++;
      cout << ".";
      fflush(stdout);

    }
    totHICAnal++;
    if(totHICAnal%50 == 0) cout << totHICAnal;
    fflush(stdout);
    if(totHICAnal%50 == 0) { // Renew the db credentials
      db = 0;
      db = initAlpideDB();
      activityDB = 0;
      activityDB = new ActivityDB(db);
    }
  }


  // Close the ROOT file and exit
  hicQualTree->Write();
  hicRecpTree->Write();
  hicHSTree->Write();
  hicStaveQualTree->Write();
  hicStaveRecpTree->Write();
  hicQualResTree->Write();
  hicRecpResTree->Write();
  hicHSResTree->Write();
  hicStaveQualResTree->Write();
  hicStaveRecpResTree->Write();
  actFastListTree->Write();
  CloseRootFile(newPowtestFile);

#ifdef USENCURSES
  mvprintw(LINES-4, 0, "\n ROOT file %s filled with %d activities\n", rootFileName.Data(), totActAnal);
#else
  printf("\n\n ROOT file %s filled with %d activities\n", rootFileName.Data(), totActAnal);
#endif
  f12ToExit();
}

void analyzePowerTest(const int hicid, const ComponentDB::compActivity act, AlpideDB *db, const THicType hicType)
{
//
// Steering routine to analyze Power Test data for one HIC
//
// Inputs:
//          hicid   : the HIC id
//          act     : the HIC activity whose Powtest will be analyzed
//          db      : a pointer to the Alpide DB
//          hicType : the HIC type (IB or OB)
//
// Outputs:
//
// Return:
//
// Created:      27 Sep 2018  Mario Sitta
// Updated:      27 Sep 2018  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
// Updated:      08 Mar 2019  Mario Sitta  HIC position & Flag ML/OL staves
// Updated:      06 Jun 2019  Mario Sitta  Get rid of timestamp from act name
//

  // Should never happen (the caller should have created it for us)
  if (!db) {
    printMessage("analyzePowerTest","Error: the DataBase was not opened");
    f12ToExit();
    return;
  }

  ActivityDB *activityDB = new ActivityDB(db);

  // Get the proper path to the Power Test result files
  ActivityDB::activityLong actLong;
  activityDB->Read(act.ID, &actLong);

  string eosPath = FindEOSPath(actLong, hicType);
  if (eosPath == "") {
    printMessage("\nanalyzePowerTest","Error: no valid EOS path found");
    f12ToExit();
    return;
  }

  // Open the ROOT file
  string rootFileName;
  if (hicType == HIC_IB)
    rootFileName = act.Name.substr(act.Name.find("IBHIC"));
  else
    rootFileName = act.Name.substr(act.Name.find("OBHIC"));

  size_t beginTS = rootFileName.find("[");
  size_t endTS   = rootFileName.find("]");
  if (beginTS != string::npos && endTS != string::npos) // Remove the timestamp
    rootFileName.erase(beginTS - 1); // Till end of string (-1 is for blank space)

  replace(rootFileName.begin(), rootFileName.end(), ' ', '_');
  rootFileName += "_PowerTest.root";

  TFile *powtestFile = OpenRootFile(rootFileName, kTRUE);
  if (!powtestFile) {
    printMessage("\nanalyzePowerTest","Error: error opening the ROOT file");
    f12ToExit();
    return;
  }

  // Create the trees
  TTree *powtestTree = CreateTreePowerTest("powtestTree","PowerTestTree");
  if (!powtestTree) {
    printMessage("\nanalyzePowerTest","Error: error creating the ROOT tree");
    f12ToExit();
    return;
  }

  TTree *powresulTree = CreateTreePowerTestResult("powresulTree","PowerTestResulTree");
  if (!powresulTree) {
    printMessage("\nanalyzePowerTest","Error: error creating the ROOT tree");
    f12ToExit();
    return;
  }

  // Fill the trees for all chips
  staveOLML = 0;
  if(actLong.Type.Name.find("HS") != string::npos &&
     (actLong.Type.Name.find("ML") != string::npos ||
      actLong.Type.Name.find("OL") != string::npos)) {
    if(actLong.Type.Name.find("ML") != string::npos)
      staveOLML = 1;
    else
      staveOLML = 2;
  }
  if(actLong.Type.Name.find("Stave") != string::npos &&
     (actLong.Type.Name.find("ML") != string::npos ||
      actLong.Type.Name.find("OL") != string::npos)) {
    if(actLong.Type.Name.find("ML") != string::npos)
      staveOLML = 1;
    else
      staveOLML = 2;
  }

  hicPosition = DbGetPosition(db, hicid);
  PowerTestAllChips(powtestTree, actLong, hicid, act.ID, eosPath);
  PowerTestResults(powresulTree, actLong, hicid, act.ID, eosPath, hicType);

  // Close the ROOT file and exit
  powtestTree->Write();
  powresulTree->Write();
  CloseRootFile(powtestFile);

#ifdef USENCURSES
  printw("\n ROOT file %s filled\n", rootFileName.c_str());
#else
  printf("\n ROOT file %s filled\n", rootFileName.c_str());
#endif
  f12ToExit();

}

void CopyPowTestOldToNew(const UInt_t hicid, const UInt_t actid,
                         TTree *newscan, TTree *newres,
                         TTree *oldscan, TTree *oldres)
{
//
// Copies all data of a given activity from the old tree to the new tree
//
// Inputs:
//          hicid   : the HIC Id
//          actid   : the Activity Id
//          newscan : the new scan tree
//          newres  : the new result tree
//          oldscan : the old scan tree
//          oldres  : the old result tree
//
// Outputs:
//
// Return:
//
// Created:      25 Jan 2019  Mario Sitta
//

  // Save current values (they were filled by FindActivityInPowTestTree
  // which was called just before us), then update
  Long64_t offsetest = testOffset;
  Long64_t offsetres = testResOffset;

  testOffset = newscan->GetEntries();
  testResOffset = newres->GetEntries();

  // Copy scan data from old tree to new tree
  Long64_t currEntries = oldscan->GetEntries();
  Int_t i = 0;
  oldscan->GetEntry(offsetest + i); // Get the first entry, then loop
  while(hicID == hicid && actID == actid && (offsetest + i) < currEntries) {
    newscan->Fill();
    i++;
    oldscan->GetEntry(offsetest + i);
  }

  // Copy condition data from old tree to new tree
  currEntries = oldres->GetEntries();
  i = 0;
  oldres->GetEntry(offsetres + i); // Ditto
  while(hicID == hicid && actID == actid && (offsetres + i) < currEntries) {
    newres->Fill();
    i++;
    oldres->GetEntry(offsetres + i);
  }

  // Reset values (to fill new activity tree) then exit
  hicID = hicid;
  actID = actid;

  return;
}

TTree* CreateHicActListTreePT(void)
{
//
// Creates an ancillary tree to store a list of HICs and Activity IDs
// in order to fast search whether a given HIC/Activity is already present
//
// Inputs:
//
// Outputs:
//
// Return:
//          a pointer to the created ROOT tree
//
// Created:      27 Nov 2018  Mario Sitta
// Updated:      25 Jan 2019  Mario Sitta
//

  TTree *newTree = 0;
  newTree = new TTree("actFastListTree", "HicActFastListTree");

  if(newTree) {
    newTree->Branch("hicID", &hicID, "hicID/i");
    newTree->Branch("actID", &actID, "actID/i");
    newTree->Branch("actMask", &actMask,"actMask/s");
    newTree->Branch("actOffs", &testOffset, "testOffset/L");
    newTree->Branch("actResOff", &testResOffset, "testResOffset/L");
  }

  return newTree;
}

TTree* CreateTreePowerTest(TString treeName, TString treeTitle)
{
//
// Creates a tree for the Power Test
//
// Inputs:
//          treeName  : the tree name
//          treeTitle : the tree title
//
// Outputs:
//
// Return:
//          a pointer to the created ROOT tree
//
// Created:      08 Jan 2019  Mario Sitta
// Updated:      08 Mar 2019  Mario Sitta  HIC position & Flag ML/OL staves
//

  TTree *newTree = 0;
  newTree = new TTree(treeName.Data(), treeTitle.Data());

  if(newTree) {
    newTree->Branch("hicID", &hicID, "hicID/i");
    newTree->Branch("actID", &actID, "actID/i");
    newTree->Branch("locID", &locID, "locID/I");
    newTree->Branch("hicPos", &hicPosition, "hicPosition/B");
    newTree->Branch("staveOLML", &staveOLML, "staveOLML/b");
    newTree->Branch("ivCurrent", ivCurrent, "ivCurrent[41]/F");
  }

  return newTree;
}

TTree* CreateTreePowerTestResult(TString treeName, TString treeTitle)
{
//
// Creates a tree for the Power Test Result
//
// Inputs:
//          treeName  : the tree name
//          treeTitle : the tree title
//
// Outputs:
//
// Return:
//          a pointer to the created ROOT tree
//
// Created:      08 Jan 2019  Mario Sitta
// Updated:      08 Mar 2019  Mario Sitta  HIC position & Flag ML/OL staves
//

  TTree *newTree = 0;
  newTree = new TTree(treeName.Data(), treeTitle.Data());

  if(newTree) {
    newTree->Branch("hicID", &hicID, "hicID/i");
    newTree->Branch("actID", &actID, "actID/i");
    newTree->Branch("locID", &locID, "locID/I");
    newTree->Branch("startDate", &startDate, "startDate/l");
    newTree->Branch("hicPos", &hicPosition, "hicPosition/B");
    newTree->Branch("staveOLML", &staveOLML, "staveOLML/b");
    newTree->Branch("vdddStart", &vdddStart, "vdddStart/F");
    newTree->Branch("vdddEnd", &vdddEnd, "vdddEnd/F");
    newTree->Branch("vddaStart", &vddaStart, "vddaStart/F");
    newTree->Branch("vddaEnd", &vddaEnd, "vddaEnd/F");
    newTree->Branch("vdddSetStart", &vdddSetStart, "vdddSetStart/F");
    newTree->Branch("vdddSetEnd", &vdddSetEnd, "vdddSetEnd/F");
    newTree->Branch("vddaSetStart", &vddaSetStart, "vddaSetStart/F");
    newTree->Branch("vddaSetEnd", &vddaSetEnd, "vddaSetEnd/F");
    newTree->Branch("idddStart", &idddStart, "idddStart/F");
    newTree->Branch("idddEnd", &idddEnd, "idddEnd/F");
    newTree->Branch("iddaStart", &iddaStart, "iddaStart/F");
    newTree->Branch("iddaEnd", &iddaEnd, "iddaEnd/F");
    newTree->Branch("anaSupVoltStart", &anaSupVoltStart, "anaSupVoltStart/F");
    newTree->Branch("anaSupVoltEnd", &anaSupVoltEnd, "anaSupVoltEnd/F");
    newTree->Branch("digSupVoltStart", &digSupVoltStart, "digSupVoltStart/F");
    newTree->Branch("digSupVoltEnd", &digSupVoltEnd, "digSupVoltEnd/F");
    newTree->Branch("tempStart", &tempStart, "tempStart/F");
    newTree->Branch("tempEnd", &tempEnd, "tempEnd/F");
    newTree->Branch("chipAnalVoltStart", chipAnalVoltStart, "chipAnalVoltStart[14]/F");
    newTree->Branch("chipAnalVoltEnd", chipAnalVoltEnd, "chipAnalVoltEnd[14]/F");
    newTree->Branch("chipDigiVoltStart", chipDigiVoltStart, "chipDigiVoltStart[14]/F");
    newTree->Branch("chipDigiVoltEnd", chipDigiVoltEnd, "chipDigiVoltEnd[14]/F");
    newTree->Branch("chipTempStart", chipTempStart, "chipTempStart[14]/F");
    newTree->Branch("chipTempEnd", chipTempEnd, "chipTempEnd[14]/F");
    newTree->Branch("idddSwitchOn", &idddAtSwitchOn, "idddAtSwitchOn/F");
    newTree->Branch("iddaSwitchOn", &iddaAtSwitchOn, "iddaAtSwitchOn/F");
    newTree->Branch("idddWClock", &idddWithClock, "idddWithClock/F");
    newTree->Branch("iddaWClock", &iddaWithClock, "iddaWithClock/F");
    newTree->Branch("idddConfig", &idddConfigured, "idddConfigured/F");
    newTree->Branch("iddaConfig", &iddaConfigured, "iddaConfigured/F");
    newTree->Branch("ibias0V", &ibias0V, "ibias0V/F");
    newTree->Branch("ibias3V", &ibias3V, "ibias3V/F");
    newTree->Branch("reg700Start", reg700Start, "reg700Start[14]/s");
    newTree->Branch("reg700End", reg700End, "reg700End[14]/s");
    newTree->Branch("classificVers", &classificVers, "classificVers/F");
    newTree->Branch("classificPowTest", &classificPowTest, "classificPowTest/F");
//    newTree->Branch("numWorkChips", &numWorkChips, "numWorkChips/F");
  }

  return newTree;
}

void PowerTestAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath)
{
//
// Loops on chips and fills the tree for the given activity
//
// Inputs:
//          ftree   : the tree to be filled
//          actlong : the activityLong for which the analysis is done
//          hicid   : the HIC id
//          actid   : the activity id
//          eospath : the input file path on EOS
//
// Outputs:
//
// Return:
//
// Created:      09 Oct 2018  Mario Sitta
// Updated:      06 Nov 2018  Mario Sitta
// Updated:      25 Jan 2019  Mario Sitta
//

  hicID = hicid;
  actID = actid;
  locID = actlong.Location.ID;

  // Open I-V Current file and fill the tree
  string dataName, resultName;
  bool ivFound;

  if(GetPowerFileName(actlong, ivFound, dataName, resultName)) {
    FillPowTestTree(ftree, eospath, dataName);
  }

}

void PowerTestResults(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType)
{
//
// Fills the tree for the given activity results
//
// Inputs:
//          ftree   : the tree to be filled
//          actlong : the activityLong for which the analysis is done
//          hicid   : the HIC id
//          actid   : the activity id
//          eospath : the input file path on EOS
//          hicType : the HIC type (IB or OB)
//
// Outputs:
//
// Return:
//
// Created:      08 Jan 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
//

  hicID = hicid;
  actID = actid;
  locID = actlong.Location.ID;
  startDate = (ulong)actlong.StartDate;

  // Open Power Test Result files and fill the tree
  string dataName, resultName;
  bool ivFound;

  if(GetPowerFileName(actlong, ivFound, dataName, resultName)) {
//    condVB = 100;
    FillPowTestTreeResult(ftree, eospath, resultName, actlong, hicType);
  }

}

Bool_t FillPowTestTree(TTree *tree, string path, string file)
{
//
// Opens the IV Current file and fills the tree
//
// Inputs:
//          tree  : the pointer to the tree to be filled
//          path  : the input file path
//          file  : the input file name
//
// Outputs:
//
// Return:
//          true if the input file was read without error, otherwise false
//
// Created:      25 Jan 2019  Mario Sitta
//

  FILE*  infile;
  string fullName;
  float  voltage, current;

  fullName = path + "/" + file;

  infile = fopen(fullName.c_str(),"r");
  if (!infile) {
    printMessage("FillPowTestTree","Warning: cannot open input file",file.c_str());
    return kFALSE;
  }

  for(Int_t i = 0; i < NUMIVPOINTS; i++)
    ivCurrent[i] = 0.;

  for(Int_t i = 0; i < NUMIVPOINTS; i++) {
    if(fscanf(infile,"%f %f",&voltage,&current) != EOF) // Should never happen
      ivCurrent[i] = current;
    else
      break;
  } // for(i < NUMIVPOINTS)

  tree->Fill();

  fclose(infile);

  return kTRUE;
}

Bool_t FillPowTestTreeResult(TTree *tree, string path, string file, ActivityDB::activityLong actlong, const THicType hicType)
{
//
// Opens the PowerTestResult file and fills the tree
// (Implementation is a bit ugly since there are many formats
// with a different number of lines and in different order)
//
// Inputs:
//          tree  : the pointer to the tree to be filled
//          path  : the input file path
//          file  : the input file name
//          actlong : the activityLong for which the analysis is done
//          hicType : the HIC type (IB or OB)
//
// Outputs:
//
// Return:
//          true if the input file was read without error, otherwise false
//
// Created:      08 Jan 2019  Mario Sitta
// Updated:      12 Jan 2019  Mario Sitta
// Updated:      25 Jan 2019  Mario Sitta
// Updated:      05 Feb 2019  Mario Sitta  Bug fix in reading registers
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
//

  FILE*  infile;
  string fullName;
  float value, dummy;
  unsigned int reg, regval;
  int ichip;
  char *line = NULL;
  size_t len = 0;

  fullName = path + "/" + file;

  infile = fopen(fullName.c_str(),"r");
  if (!infile) {
    printMessage("FillPowTestTreeResult","Warning: cannot open input file",file.c_str());
    return kFALSE;
  }

  ResetPowTestTreeVariables();

  for (int j=0; j<3; j++) // Skip first three lines
    getline(&line, &len, infile);

  // Scan the file, search for data
  // (file format not unique, lines can be in any order)
  bool chipStartFound = false, chipEndFound = false;
  while(getline(&line, &len, infile) > 0) {

    if (strlen(line) < 2) continue; // Blank line
    if (strstr(line,"Board registers")) break; // We've finished

    if (strstr(line,"Chip registers (start)")) {
      chipStartFound = true;
      chipEndFound = false;
      continue;
    }
    if (strstr(line,"Chip registers (end)")) {
      chipStartFound = false;
      chipEndFound = true;
      continue;
    }

    if(chipStartFound || chipEndFound) goto chipRegisters; // OMG, a goto!!!

    // General data
    if (strstr(line,"VDDD (start)")) {
      sscanf(line, "VDDD (start): %f", &vdddStart);
      continue;
    }

    if (strstr(line,"VDDD (end)")) {
      sscanf(line, "VDDD (end):   %f", &vdddEnd);
      continue;
    }

    if (strstr(line,"VDDA (start)")) {
      sscanf(line, "VDDA (start): %f", &vddaStart);
      continue;
    }

    if (strstr(line,"VDDA (end)")) {
      sscanf(line, "VDDA (end):   %f", &vddaEnd);
      continue;
    }

    if (strstr(line,"VDDD set (start)")) {
      sscanf(line, "VDDD set (start): %f", &vdddSetStart);
      continue;
    }

    if (strstr(line,"VDDD set (end)")) {
      sscanf(line, "VDDD set (end):   %f", &vdddSetEnd);
      continue;
    }

    if (strstr(line,"VDDA set (start)")) {
      sscanf(line, "VDDA set (start): %f", &vddaSetStart);
      continue;
    }

    if (strstr(line,"VDDA set (end)")) {
      sscanf(line, "VDDA set (end):   %f", &vddaSetEnd);
      continue;
    }

    if (strstr(line,"IDDD (start)")) {
      sscanf(line, "IDDD (start): %f", &idddStart);
      continue;
    }

    if (strstr(line,"IDDD (end)")) {
      sscanf(line, "IDDD (end):   %f", &idddEnd);
      continue;
    }

    if (strstr(line,"IDDA (start)")) {
      sscanf(line, "IDDA (start): %f", &iddaStart);
      continue;
    }

    if (strstr(line,"IDDA (end)")) {
      sscanf(line, "IDDA (end):   %f", &iddaEnd);
      continue;
    }

    if (strstr(line,"Analogue Supply Voltage") && strstr(line,"start")) {
      if (strstr(line,"on-chip"))
        sscanf(line, "Analogue Supply Voltage (on-chip, start): %f", &anaSupVoltStart);
      else
        sscanf(line, "Analogue Supply Voltage (start): %f", &anaSupVoltStart);
      continue;
    }

    if (strstr(line,"Analogue Supply Voltage") && strstr(line,"end")) {
      if (strstr(line,"on-chip"))
        sscanf(line, "Analogue Supply Voltage (on-chip, end):   %f", &anaSupVoltEnd);
      else
        sscanf(line, "Analogue Supply Voltage (end):   %f", &anaSupVoltEnd);
      continue;
    }

    if (strstr(line,"Digital Supply Voltage") && strstr(line,"start")) {
      if (strstr(line,"on-chip"))
        sscanf(line, "Digital Supply Voltage (on-chip, saturating at %fV, start): %f", &dummy, &digSupVoltStart);
      else
        sscanf(line, "Digital Supply Voltage (saturating at %fV, start): %f", &dummy, &digSupVoltStart);
      continue;
    }

    if (strstr(line,"Digital Supply Voltage") && strstr(line,"end")) {
      if (strstr(line,"on-chip"))
        sscanf(line, "Digital Supply Voltage (on-chip, saturating at %fV, end):   %f", &dummy, &digSupVoltEnd);
      else
        sscanf(line, "Digital Supply Voltage (saturating at %fV, end):   %f", &dummy, &digSupVoltEnd);
      continue;
    }

    if (strstr(line,"Temp (") && strstr(line,"start")) {
      if (strstr(line,"on-chip"))
        sscanf(line,"Temp (on-chip, start): %f",&tempStart);
      else
        sscanf(line,"Temp (start): %f",&tempStart);
      continue;
    }

    if (strstr(line,"Temp (") && strstr(line,"end")) {
      if (strstr(line,"on-chip"))
        sscanf(line,"Temp (on-chip, end):   %f",&tempEnd);
      else
        sscanf(line,"Temp (end):   %f",&tempEnd);
      continue;
    }

    // Per-chip data
    if (strstr(line,"Analogue voltage (") && strstr(line,"start")) {
      sscanf(line, "  Analogue voltage (start) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
	chipAnalVoltStart[ichip-1] = value; 
      else
	chipAnalVoltStart[ichip] = value;
    }

    if (strstr(line,"Analogue voltage (") && strstr(line,"end")) {
      sscanf(line, "  Analogue voltage (end) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
	chipAnalVoltEnd[ichip-1] = value;
      else
	chipAnalVoltEnd[ichip] = value;
    }

    if (strstr(line,"Digital voltage (") && strstr(line,"start")) {
      sscanf(line, "  Digital voltage (start) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
        chipDigiVoltStart[ichip-1] = value;
      else
        chipDigiVoltStart[ichip] = value;
    }

    if (strstr(line,"Digital voltage (") && strstr(line,"end")) {
      sscanf(line, "  Digital voltage (end) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
        chipDigiVoltEnd[ichip-1] = value;
      else
        chipDigiVoltEnd[ichip] = value;
    }

    if (strstr(line,"Temperature (") && strstr(line,"start")) {
      sscanf(line, "  Temperature (start) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
	chipTempStart[ichip-1] = value;
      else
	chipTempStart[ichip] = value;
    }

    if (strstr(line,"Temperature (") && strstr(line,"end")) {
      sscanf(line, "  Temperature (end) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
	chipTempEnd[ichip-1] = value;
      else
	chipTempEnd[ichip] = value;
    }

    // Specific Power Test data
    if (strstr(line,"IDDD at switchon")) {
      sscanf(line, "IDDD at switchon:  %f", &idddAtSwitchOn);
      continue;
    }

    if (strstr(line,"IDDA at switchon")) {
      sscanf(line, "IDDA at switchon:  %f", &iddaAtSwitchOn);
      continue;
    }

    if (strstr(line,"IDDD with clock")) {
      sscanf(line, "IDDD with clock:   %f", &idddWithClock);
      continue;
    }

    if (strstr(line,"IDDA with clock")) {
      sscanf(line, "IDDA with clock:   %f", &iddaWithClock);
      continue;
    }

    if (strstr(line,"IDDD configured")) {
      sscanf(line, "IDDD configured:   %f", &idddConfigured);
      continue;
    }

    if (strstr(line,"IDDA configured")) {
      sscanf(line, "IDDA configured:   %f", &iddaConfigured);
      continue;
    }

    if (strstr(line,"IBias at 0V")) {
      sscanf(line, "IBias at 0V:       %f", &ibias0V);
      continue;
    }

    if (strstr(line,"IBias at 3V")) {
      sscanf(line, "IBias at 3V:       %f", &ibias3V);
      continue;
    }

    // Registers data
  chipRegisters:
    if (strstr(line,"# Chip ID"))
      continue;

    sscanf(line, "%d        0x%x        0x%x", &ichip, &reg, &regval);

    if(reg == 0x700) {
      ichip &= 0x0f; // ichip is the lower 4 bits
      if(chipStartFound) {
	if(hicType == HIC_OB && ichip > 7)
          reg700Start[ichip-1] = regval;
        else
          reg700Start[ichip] = regval;
      } else {
	if(hicType == HIC_OB && ichip > 7)
          reg700End[ichip-1] = regval;
        else
          reg700End[ichip] = regval;
      }
    }

  } // while(getline(&line, &len, infile))

  fclose(infile);

  // Now get data from DB
  vector<ActivityDB::actParameter>  parameters = actlong.Parameters;

  std::vector<ActivityDB::actParameter>::iterator loop;
  for(loop = parameters.begin(); loop != parameters.end(); loop++) {
    ActivityDB::actParameter actpar = *loop;

    if(actpar.Type.Parameter.Name.compare("Classification Version") == 0) {
      classificVers = actpar.Value;
      continue;
    }

    if(actpar.Type.Parameter.Name.compare("Classification Power Test") == 0) {
      classificPowTest = actpar.Value;
      continue;
    }

//    if(actpar.Type.Parameter.Name.compare("Number of Working Chips") == 0) {
//      numWorkChips = actpar.Value;
//      continue;
//    }
    
  }

  // Fill the tree, close the file and return
  tree->Fill();

  return kTRUE;
}

Bool_t FindActivityInPowTestTree(TTree* listree, const UInt_t hicid, const UInt_t actid, const UShort_t mask)
{
//
// Finds the activity in the tree
//
// Inputs:
//          listree : the tree with the list of activities
//          hicid : the HIC Id
//          actid : the Activity Id
//          mask  : the activity mask
//
// Outputs:
//
// Return:
//          true if activity found
//
// Created:      09 Jan 2019  Mario Sitta
//

  Bool_t found = kFALSE;

  // Save variable values before scanning the tree
  // (variable names are necessarily the same,
  // their value will change during scan)
  Int_t currHicId = hicID;
  Int_t currActId = actID;
  UShort_t currMask = actMask;
  
  Int_t nEntries = listree->GetEntries();
  for (Int_t j = 0; j < nEntries; j++) {
    listree->GetEntry(j);
    if(hicID == hicid && actID == actid && actMask == mask) {
      found = kTRUE;
      break;
    }
  }

  // Restore values
  hicID = currHicId;
  actID = currActId;
  actMask = currMask;

  return found;
}

TTree* ReadHicActListTreePT(TFile *rootfile)
{
//
// Reads the ancillary tree from the Root file
// WARNING!! We assume the rootfile was already successfully opened!
// NO checks on file!
//
// Inputs:
//          rootfile : the Root file
//
// Outputs:
//
// Return:
//          a pointer to the read ROOT tree
//
// Created:      28 Nov 2018  Mario Sitta
// Updated:      25 Jan 2019  Mario Sitta
//

  TTree *newtree = 0;
  newtree = (TTree*)rootfile->Get("actFastListTree");

  if(newtree) {
    newtree->SetBranchAddress("hicID", &hicID);
    newtree->SetBranchAddress("actID", &actID);
    newtree->SetBranchAddress("actMask", &actMask);
    newtree->SetBranchAddress("actOffs", &testOffset);
    newtree->SetBranchAddress("actResOff", &testResOffset);
  }

  return newtree;
}

TTree* ReadPowTestTree(TString treename, TFile *rootfile)
{
//
// Reads a tree for Power Test from file
// WARNING!! We assume the rootfile was already successfully opened!
// NO checks on file!
//
// Inputs:
//          treename : the tree name
//          rootfile : the Root file
//
// Outputs:
//
// Return:
//          a pointer to the read ROOT tree
//
// Created:      09 Jan 2019  Mario Sitta
// Updated:      25 Jan 2019  Mario Sitta
// Updated:      08 Mar 2019  Mario Sitta  HIC position & Flag ML/OL staves
//

  TTree *newtree = 0;
  newtree = (TTree*)rootfile->Get(treename.Data());

  if(newtree) {
    newtree->SetBranchAddress("hicID", &hicID);
    newtree->SetBranchAddress("actID", &actID);
    newtree->SetBranchAddress("locID", &locID);
    newtree->SetBranchAddress("hicPos", &hicPosition);
    newtree->SetBranchAddress("staveOLML", &staveOLML);
    newtree->SetBranchAddress("ivCurrent", ivCurrent);
  }

  return newtree;
}

TTree* ReadPowTestTreeResult(TString treename, TFile *rootfile)
{
//
// Reads a tree for Power Test from file
// WARNING!! We assume the rootfile was already successfully opened!
// NO checks on file!
//
// Inputs:
//          treename : the tree name
//          rootfile : the Root file
//
// Outputs:
//
// Return:
//          a pointer to the read ROOT tree
//
// Created:      09 Jan 2019  Mario Sitta
//

  TTree *newtree = 0;
  newtree = (TTree*)rootfile->Get(treename.Data());

  if(newtree) {
    newtree->SetBranchAddress(            "hicID", &hicID);
    newtree->SetBranchAddress(            "actID", &actID);
    newtree->SetBranchAddress(            "locID", &locID);
    newtree->SetBranchAddress(        "startDate", &startDate);
    newtree->SetBranchAddress(           "hicPos", &hicPosition);
    newtree->SetBranchAddress(        "staveOLML", &staveOLML);
    newtree->SetBranchAddress(        "vdddStart", &vdddStart);
    newtree->SetBranchAddress(          "vdddEnd", &vdddEnd);
    newtree->SetBranchAddress(        "vddaStart", &vddaStart);
    newtree->SetBranchAddress(          "vddaEnd", &vddaEnd);
    newtree->SetBranchAddress(     "vdddSetStart", &vdddSetStart);
    newtree->SetBranchAddress(       "vdddSetEnd", &vdddSetEnd);
    newtree->SetBranchAddress(     "vddaSetStart", &vddaSetStart);
    newtree->SetBranchAddress(       "vddaSetEnd", &vddaSetEnd);
    newtree->SetBranchAddress(        "idddStart", &idddStart);
    newtree->SetBranchAddress(          "idddEnd", &idddEnd);
    newtree->SetBranchAddress(        "iddaStart", &iddaStart);
    newtree->SetBranchAddress(          "iddaEnd", &iddaEnd);
    newtree->SetBranchAddress(  "anaSupVoltStart", &anaSupVoltStart);
    newtree->SetBranchAddress(    "anaSupVoltEnd", &anaSupVoltEnd);
    newtree->SetBranchAddress(  "digSupVoltStart", &digSupVoltStart);
    newtree->SetBranchAddress(    "digSupVoltEnd", &digSupVoltEnd);
    newtree->SetBranchAddress(        "tempStart", &tempStart);
    newtree->SetBranchAddress(          "tempEnd", &tempEnd);
    newtree->SetBranchAddress("chipAnalVoltStart",  chipAnalVoltStart);
    newtree->SetBranchAddress(  "chipAnalVoltEnd",  chipAnalVoltEnd);
    newtree->SetBranchAddress("chipDigiVoltStart",  chipDigiVoltStart);
    newtree->SetBranchAddress(  "chipDigiVoltEnd",  chipDigiVoltEnd);
    newtree->SetBranchAddress(    "chipTempStart",  chipTempStart);
    newtree->SetBranchAddress(      "chipTempEnd",  chipTempEnd);
    newtree->SetBranchAddress(     "idddSwitchOn", &idddAtSwitchOn);
    newtree->SetBranchAddress(     "iddaSwitchOn", &iddaAtSwitchOn);
    newtree->SetBranchAddress(       "idddWClock", &idddWithClock);
    newtree->SetBranchAddress(       "iddaWClock", &iddaWithClock);
    newtree->SetBranchAddress(       "idddConfig", &idddConfigured);
    newtree->SetBranchAddress(       "iddaConfig", &iddaConfigured);
    newtree->SetBranchAddress(          "ibias0V", &ibias0V);
    newtree->SetBranchAddress(          "ibias3V", &ibias3V);
    newtree->SetBranchAddress(      "reg700Start",  reg700Start);
    newtree->SetBranchAddress(        "reg700End",  reg700End);
    newtree->SetBranchAddress(    "classificVers", &classificVers);
    newtree->SetBranchAddress( "classificPowTest", &classificPowTest);
//    newtree->SetBranchAddress(     "numWorkChips", &numWorkChips);
  }

  return newtree;
}

void ResetPowTestTreeVariables(void)
{
//
// Resets the variables of the Result tree
//
// Inputs:
//
// Outputs:
//
// Return:
//
// Created:      09 Jan 2019  Mario Sitta
//

  vdddStart = 0;
  vdddEnd = 0;
  vddaStart = 0;
  vddaEnd = 0;
  vdddSetStart = 0;
  vdddSetEnd = 0;
  vddaSetStart = 0;
  vddaSetEnd = 0;
  idddStart = 0;
  idddEnd = 0;
  iddaStart = 0;
  iddaEnd = 0;
  anaSupVoltStart = 0;
  anaSupVoltEnd = 0;
  digSupVoltStart = 0;
  digSupVoltEnd = 0;
  tempStart = 0;
  tempEnd = 0;
  for (int i = 0; i < NUMCHIPS; i++) {
    chipAnalVoltStart[i] = 0;
    chipAnalVoltEnd[i] = 0;
    chipDigiVoltStart[i] = 0;
    chipDigiVoltEnd[i] = 0;
    chipTempStart[i] = 0;
    chipTempEnd[i] = 0;
  }
  idddAtSwitchOn = 0;
  iddaAtSwitchOn = 0;
  idddWithClock = 0;
  iddaWithClock = 0;
  idddConfigured = 0;
  iddaConfigured = 0;
  ibias0V = 0;
  ibias3V = 0;
  classificVers = 0;
  classificPowTest = 0;
//  numWorkChips = 0;
}

TTree* SetupHicActListTreePT(TFile *rootfile)
{
//
// Creates a new tree or reads it from file
//
// Inputs:
//          rootfile  : the (already opened) Root file
//
// Outputs:
//
// Return:
//          a pointer to the created/read tree
//
// Created:      28 Nov 2018  Mario Sitta
//

  TTree *newtree = 0;
  
  if(redoFromStart)
    newtree = CreateHicActListTreePT();
  else
    newtree = ReadHicActListTreePT(rootfile);

  return newtree;
}

TTree* SetupPowTestTree(TString treename, TString treetitle, TFile *rootfile)
{
//
// Creates a new tree or reads it from file
//
// Inputs:
//          treename  : the tree name
//          treetitle : the tree title
//          rootfile  : the (already opened) Root file
//
// Outputs:
//
// Return:
//          a pointer to the created/read tree
//
// Created:      28 Nov 2018  Mario Sitta
//

  TTree *newtree = 0;
  
  if(redoFromStart)
    newtree = CreateTreePowerTest(treename,treetitle);
  else
    newtree = ReadPowTestTree(treename, rootfile);

  return newtree;
}

TTree* SetupPowTestTreeResult(TString treename, TString treetitle, TFile *rootfile)
{
//
// Creates a new tree or reads it from file
//
// Inputs:
//          treename  : the tree name
//          treetitle : the tree title
//          rootfile  : the (already opened) Root file
//
// Outputs:
//
// Return:
//          a pointer to the created/read tree
//
// Created:      28 Nov 2018  Mario Sitta
//

  TTree *newtree = 0;
  
  if(redoFromStart)
    newtree = CreateTreePowerTestResult(treename,treetitle);
  else
    newtree = ReadPowTestTreeResult(treename, rootfile);

  return newtree;
}


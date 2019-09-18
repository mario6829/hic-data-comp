#include "threscanlib.h"
#include "utillib.h"
#include "menulib.h"
#include "treevariables.h"

// Local tree variables
static UShort_t colNum;
static UShort_t rowNum;
Int_t    n8b10bErrors;
Int_t    corruptEvents;
Int_t    oversizeEvts;
Int_t    timeouts;
Int_t    pixWOHits[NUMCHIPS];
Int_t    pixWOThres[NUMCHIPS];
Int_t    hotPixels[NUMCHIPS];
//Float_t  thresValue;
//Float_t  noiseValue;
UShort_t  thresValue;
UShort_t  noiseValue;
Float_t  avrgThres[NUMCHIPS];
Float_t  thresRMS[NUMCHIPS];
Float_t  deviation[NUMCHIPS];
Float_t  avrgNoise[NUMCHIPS];
Float_t  noiseRMS[NUMCHIPS];
Int_t    classificThreScan;
static Long64_t testTunOffset;


void analyzeAllThresholdScans(std::vector<ComponentDB::componentShort> componentList, AlpideDB *db, const THicType hicType)
{
//
// Steering routine to analyze the data of all Threshold Scans for all HICs
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
// Created:      29 Jan 2019  Mario Sitta
// Updated:      31 Jan 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
// Updated:      05 Jul 2019  Mario Sitta  Chip Wafer and position added
//

  // We need to define here the TTree's for the existing ROOT file
  TTree *oldHicQualTree = 0, *oldHicRecpTree = 0, *oldHicHSTree = 0, *oldHicStaveTree = 0;
  TTree *oldHicQualTunTree = 0, *oldHicRecpTunTree = 0, *oldHicHSTunTree = 0, *oldHicStaveTunTree = 0;
  TTree *oldHicQualResTree = 0, *oldHicRecpResTree = 0, *oldHicHSResTree = 0, *oldHicStaveResTree = 0;
  TTree *oldActFastListTree = 0;

  // Should never happen (the caller should have created it for us)
  if (!db) {
    printMessage("analyzeAllThresholdScans","Error: the DataBase was not opened");
    f12ToExit();
    return;
  }

  ActivityDB *activityDB = new ActivityDB(db);

  // Check whether the ROOT file already exists
  // If yes, ask the user whether to use it or redo a new one
  TString rootFileName;
  if (hicType == HIC_IB)
    rootFileName = "IBHIC_ThresholdScan_AllHICs.root";
  else
    rootFileName = "OBHIC_ThresholdScan_AllHICs.root";

  redoFromStart = kTRUE;
  if(CheckRootFileExists(rootFileName)) {
    if(AskUserRedoScan() == 2) { // User chose to re-use existing tree
      redoFromStart = kFALSE;
      TString oldRootFileName;
      if(!RenameExistingRootFile(rootFileName, "_old", oldRootFileName)) {
        printMessage("\nanalyzeAllThresholdScans","Error: error renaming existing ROOT file");
        f12ToExit();
        return;
      }

      TFile *oldThrescanFile = OpenRootFile(oldRootFileName);
      if(!oldThrescanFile) {
        printMessage("\nanalyzeAllThresholdScans","Error: error opening existing ROOT file");
        f12ToExit();
        return;
      }

      oldHicQualTree = ReadThreScanTree("hicQualTree",oldThrescanFile);
      oldHicRecpTree = ReadThreScanTree("hicRecpTree",oldThrescanFile);
      oldHicHSTree = ReadThreScanTree("hicHSTree",oldThrescanFile);
      oldHicStaveTree = ReadThreScanTree("hicStaveTree",oldThrescanFile);

      oldHicQualTunTree = ReadThreScanTree("hicQualTunTree",oldThrescanFile);
      oldHicRecpTunTree = ReadThreScanTree("hicRecpTunTree",oldThrescanFile);
      oldHicHSTunTree = ReadThreScanTree("hicHSTunTree",oldThrescanFile);
      oldHicStaveTunTree = ReadThreScanTree("hicStaveTunTree",oldThrescanFile);

      oldHicQualResTree = ReadThreScanTreeResult("hicQualResTree",oldThrescanFile);
      oldHicRecpResTree = ReadThreScanTreeResult("hicRecpResTree",oldThrescanFile);
      oldHicHSResTree = ReadThreScanTreeResult("hicHSResTree",oldThrescanFile);
      oldHicStaveResTree = ReadThreScanTreeResult("hicStaveResTree",oldThrescanFile);

      oldActFastListTree = ReadHicActListTreeTS(oldThrescanFile);

      if(!oldHicQualTree || !oldHicRecpTree || !oldHicHSTree || !oldHicStaveTree ||
         !oldHicQualTunTree || !oldHicRecpTunTree || !oldHicHSTunTree || !oldHicStaveTunTree ||
         !oldHicQualResTree || !oldHicRecpResTree || !oldHicHSResTree || !oldHicStaveResTree ||
         !oldActFastListTree) {
        printMessage("\nanalyzeAllThresholdScans","Error: error reading trees from existing ROOT file");
        f12ToExit();
        return;
      }
    } // if(AskUserRedoScan())
  } // if(CheckRootFileExists())

  // Open the ROOT file
  TFile *newThrescanFile = OpenRootFile(rootFileName, kTRUE);

  if(!newThrescanFile) {
    printMessage("\nanalyzeAllThresholdScans","Error: error opening new ROOT file");
    f12ToExit();
    return;
  }

  // Create or read the trees
  TTree *hicQualTree = CreateTreeThresholdScan("hicQualTree","HicQualificationTest");
  TTree *hicRecpTree = CreateTreeThresholdScan("hicRecpTree","HicReceptionTest");
  TTree *hicHSTree = CreateTreeThresholdScan("hicHSTree","HicHalfStaveTest");
  TTree *hicStaveTree = CreateTreeThresholdScan("hicStaveTree","HicStaveTest");

  TTree *hicQualTunTree = CreateTreeThresholdScan("hicQualTunTree","HicQualifTuneTest");
  TTree *hicRecpTunTree = CreateTreeThresholdScan("hicRecpTunTree","HicReceptTuneTest");
  TTree *hicHSTunTree = CreateTreeThresholdScan("hicHSTunTree","HicHalfStavTuneTest");
  TTree *hicStaveTunTree = CreateTreeThresholdScan("hicStaveTunTree","HicStaveTuneTest");

  TTree *hicQualResTree = CreateTreeThresholdScanResult("hicQualResTree","HicQualificationTestResults");
  TTree *hicRecpResTree = CreateTreeThresholdScanResult("hicRecpResTree","HicReceptionTestResults");
  TTree *hicHSResTree = CreateTreeThresholdScanResult("hicHSResTree","HicHalfStaveTestResults");
  TTree *hicStaveResTree = CreateTreeThresholdScanResult("hicStaveResTree","HicStaveTestResults");

  TTree *actFastListTree = CreateHicActListTreeTS();

  // Loop on all components
  int totHICAnal = 0, totActAnal = 0;
  std::vector<ComponentDB::compActivity> tests;
  std::vector<ComponentDB::componentShort>::iterator iComp;
  for (iComp = componentList.begin(); iComp != componentList.end(); iComp++) {
    ComponentDB::componentShort comp = *iComp;
    DbGetAllTests (db, comp.ID, tests, STThreshold, true);

    // Get the list of chips in this HIC
    std::vector<TChild> children;
    int nChildren = DbGetListOfChildren(db, comp.ID, children, true);
    if (nChildren == 0)
      printMessage("\nanalyzeThresholdScan","Warning: HIC has no children ", comp.ComponentID.c_str());

    // Loop on all activities
    std::vector<ComponentDB::compActivity>::iterator it;
    for(it = tests.begin(); it != tests.end(); it++) {
      ComponentDB::compActivity act = *it;
      ActivityDB::activityLong actLong;
      activityDB->Read(act.ID, &actLong);

      TTree *testree = 0, *testuntree = 0, *resultree = 0;
      TTree *oldtestree = 0, *oldtestuntree = 0, *oldresultree = 0;
      if(actLong.Type.Name.find("HIC") != string::npos &&
         actLong.Type.Name.find("Qualification") != string::npos) {
        testree = hicQualTree;
        testuntree = hicQualTunTree;
        resultree = hicQualResTree;
        oldtestree = oldHicQualTree;
        oldtestuntree = oldHicQualTunTree;
        oldresultree = oldHicQualResTree;
        actMask = ACTMASK_QUALIF;
      }
      if(actLong.Type.Name.find("HIC") != string::npos &&
         actLong.Type.Name.find("Reception") != string::npos) {
        testree = hicRecpTree;
        testuntree = hicRecpTunTree;
        resultree = hicRecpResTree;
        oldtestree = oldHicRecpTree;
        oldtestuntree = oldHicRecpTunTree;
        oldresultree = oldHicRecpResTree;
        actMask = ACTMASK_RECEPT;
      }
      if(actLong.Type.Name.find("HS") != string::npos &&
        (actLong.Type.Name.find("ML") != string::npos ||
         actLong.Type.Name.find("OL") != string::npos)) {
        testree = hicHSTree;
        testuntree = hicHSTunTree;
        resultree = hicHSResTree;
        oldtestree = oldHicHSTree;
        oldtestuntree = oldHicHSTunTree;
        oldresultree = oldHicHSResTree;
        actMask = ACTMASK_HALFST;
      }
      if(actLong.Type.Name.find("Stave") != string::npos &&
        (actLong.Type.Name.find("ML") != string::npos ||
         actLong.Type.Name.find("OL") != string::npos)) {
        testree = hicStaveTree;
        testuntree = hicStaveTunTree;
        resultree = hicStaveResTree;
        oldtestree = oldHicStaveTree;
        oldtestuntree = oldHicStaveTunTree;
        oldresultree = oldHicStaveResTree;
        actMask = ACTMASK_STAVET;
      }

      if (!testree) continue; // Not a Qualification/Reception/HS/Stave test

      if(!redoFromStart)
        if(FindActivityInThreScanTree(oldActFastListTree, comp.ID, act.ID, actMask)) {
          printMessage("\nanalyzeAllThresholdScans", "Activity already in file, copying trees ", actLong.Name.c_str());
          CopyThreScanOldToNew(comp.ID, act.ID, testree, testuntree, resultree, oldtestree, oldtestuntree, oldresultree);
          actFastListTree->Fill();
          continue;
        }

      string eosPath = FindEOSPath(actLong, hicType);
      if(eosPath.length() == 0) { // No valid path found on EOS
        string hicAct = actLong.Name + " " + actLong.Type.Name;
        printMessage("\nanalyzeAllThresholdScans", "EOS for this activity does not exists", hicAct.c_str());
        continue;
      }

      // Fill the tree for all chips (only post-tuning scans)
      testOffset = testree->GetEntries();
      testTunOffset = testuntree->GetEntries();
      testResOffset = resultree->GetEntries();

      hicClass = ConvertTestResult(act.Result.Name);

      ThresholdScanAllChips(testree, actLong, comp.ID, act.ID, eosPath, hicType, children, false);
      ThresholdTuneAllChips(testuntree, actLong, comp.ID, act.ID, eosPath, hicType, children);
      ThresholdScanResults(resultree, actLong, comp.ID, act.ID, eosPath, hicType);

      Long64_t prevTestOffset = testree->GetEntries();
      Long64_t prevTestTunOffset = testuntree->GetEntries();
      Long64_t prevTestResOffset = resultree->GetEntries();

      if(testOffset == prevTestOffset || testResOffset == prevTestResOffset || testTunOffset == prevTestTunOffset)
        printMessage("\nanalyzeAllThresholdScans", "Trees not filled for activity ", actLong.Name.c_str());
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
  hicStaveTree->Write();
  hicQualResTree->Write();
  hicRecpResTree->Write();
  hicHSResTree->Write();
  hicStaveResTree->Write();
  actFastListTree->Write();
  CloseRootFile(newThrescanFile);

#ifdef USENCURSES
  mvprintw(LINES-4, 0, "\n ROOT file %s filled with %d activities\n", rootFileName.Data(), totActAnal);
#else
  printf("\n\n ROOT file %s filled with %d activities\n", rootFileName.Data(), totActAnal);
#endif
  f12ToExit();
}

void analyzeThresholdScan(const int hicid, const ComponentDB::compActivity act, AlpideDB *db, const THicType hicType)
{
//
// Steering routine to analyze Threshold Scan data for one HIC
//
// Inputs:
//          hicid   : the HIC id
//          act     : the HIC activity whose DigiScan will be analyzed
//          db      : a pointer to the Alpide DB
//          hicType : the HIC type (IB or OB)
//
// Outputs:
//
// Return:
//
// Created:      28 Jan 2019  Mario Sitta
// Updated:      31 Jan 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
// Updated:      06 Jun 2019  Mario Sitta  Get rid of timestamp from act name
// Updated:      05 Jul 2019  Mario Sitta  Chip Wafer and position added
//

  // Should never happen (the caller should have created it for us)
  if (!db) {
    printMessage("analyzeThresholdScan","Error: the DataBase was not opened");
    f12ToExit();
    return;
  }

  ActivityDB *activityDB = new ActivityDB(db);

  // Get the proper path to the Digital Scan result files
  ActivityDB::activityLong actLong;
  activityDB->Read(act.ID, &actLong);

  string eosPath = FindEOSPath(actLong, hicType);
  if (eosPath == "") {
    printMessage("\nanalyzeThresholdScan","Error: no valid EOS path found");
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
  rootFileName += "_ThresholdScan.root";

  TFile *threscanFile = OpenRootFile(rootFileName, kTRUE);
  if (!threscanFile) {
    printMessage("\nanalyzeThresholdScan","Error: error opening the ROOT file");
    f12ToExit();
    return;
  }

  // Create the trees
  TTree *threscanTree = CreateTreeThresholdScan("threscanTree","ThresholdScanTree");
  if (!threscanTree) {
    printMessage("\nanalyzeThresholdScan","Error: error creating the ROOT tree");
    f12ToExit();
    return;
  }

  TTree *threstunTree = CreateTreeThresholdScan("threstunTree","ThresholdTuneTree");
  if (!threstunTree) {
    printMessage("\nanalyzeThresholdScan","Error: error creating the ROOT tree");
    f12ToExit();
    return;
  }

  TTree *thresresulTree = CreateTreeThresholdScanResult("thresresulTree","ThresholdScanResulTree");
  if (!thresresulTree) {
    printMessage("\nanalyzeThresholdScan","Error: error creating the ROOT tree");
    f12ToExit();
    return;
  }

  // Get the list of chips in this HIC
  std::vector<TChild> children;
  int nChildren = DbGetListOfChildren(db, hicid, children, true);
  if (nChildren == 0)
    printMessage("\nanalyzeThresholdScan","Warning: HIC has no children");

  // Fill the trees for all chips (all scans)
  hicClass = ConvertTestResult(act.Result.Name);
  ThresholdScanAllChips(threscanTree, actLong, hicid, act.ID, eosPath, hicType, children);
  ThresholdTuneAllChips(threstunTree, actLong, hicid, act.ID, eosPath, hicType, children);
  ThresholdScanResults(thresresulTree, actLong, hicid, act.ID, eosPath, hicType);

  // Close the ROOT file and exit
  threscanTree->Write();
  threstunTree->Write();
  thresresulTree->Write();
  CloseRootFile(threscanFile);

#ifdef USENCURSES
  printw("\n ROOT file %s filled\n", rootFileName.c_str());
#else
  printf("\n ROOT file %s filled\n", rootFileName.c_str());
#endif
  f12ToExit();

}

void CopyThreScanOldToNew(const UInt_t hicid, const UInt_t actid,
			  TTree *newscan, TTree *newtun, TTree *newres,
			  TTree *oldscan, TTree *oldtun, TTree *oldres)
{
//
// Copies all data of a given activity from the old tree to the new tree
//
// Inputs:
//          hicid   : the HIC Id
//          actid   : the Activity Id
//          newscan : the new scan tree
//          newtun  : the new tuning tree
//          newres  : the new result tree
//          oldscan : the old scan tree
//          oldtun  : the old tuning tree
//          oldres  : the old result tree
//
// Outputs:
//
// Return:
//
// Created:      18 Jan 2019  Mario Sitta
// Updated:      31 Jan 2019  Mario Sitta  We have 3 trees here
//

  // Save current values (they were filled by FindActivityInThreScanTree
  // which was called just before us), then update
  Long64_t offsetest = testOffset;
  Long64_t offsettun = testTunOffset;
  Long64_t offsetres = testResOffset;

  testOffset = newscan->GetEntries();
  testTunOffset = newtun->GetEntries();
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

  // Copy tuning data from old tree to new tree
  currEntries = oldtun->GetEntries();
  i = 0;
  oldtun->GetEntry(offsettun + i); // Ditto
  while(hicID == hicid && actID == actid && (offsettun + i) < currEntries) {
    newtun->Fill();
    i++;
    oldtun->GetEntry(offsetest + i);
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

TTree* CreateHicActListTreeTS(void)
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
// Updated:      18 Jan 2019  Mario Sitta
//

  TTree *newTree = 0;
  newTree = new TTree("actFastListTree", "HicActFastListTree");

  if(newTree) {
    newTree->Branch("hicID", &hicID, "hicID/i");
    newTree->Branch("actID", &actID, "actID/i");
    newTree->Branch("actMask", &actMask, "actMask/s");
    newTree->Branch("actOffs", &testOffset, "testOffset/L");
    newTree->Branch("actTunOff", &testTunOffset, "testTunOffset/L");
    newTree->Branch("actResOff", &testResOffset, "testResOffset/L");
  }

  return newTree;
}

TTree* CreateTreeThresholdScan(TString treeName, TString treeTitle)
{
//
// Creates a tree for the Threshold Scan
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
// Updated:      05 Jul 2019  Mario Sitta  Chip Wafer and position added
// Updated:      09 Jul 2019  Mario Sitta  HIC class added
//

  TTree *newTree = 0;
  newTree = new TTree(treeName.Data(), treeTitle.Data());

  if(newTree) {
    newTree->Branch("hicID", &hicID, "hicID/i");
    newTree->Branch("actID", &actID, "actID/i");
    newTree->Branch("locID", &locID, "locID/I");
    newTree->Branch("condVB", &condVB, "condVB/b");
    newTree->Branch("hicClass", &hicClass, "hicClass/B");
    newTree->Branch("chipNum", &chipNum, "chipNum/b");
    newTree->Branch("waferNum", &waferNum, "waferNum/B");
    newTree->Branch("waferPos", &waferPos, "waferPos/B");
    newTree->Branch("colNum", &colNum, "colNum/s");
    newTree->Branch("rowNum", &rowNum, "rowNum/s");
//    newTree->Branch("thresh", &thresValue, "thresValue/F");
//    newTree->Branch("noise", &noiseValue, "noiseValue/F");
    newTree->Branch("thresh", &thresValue, "thresValue/s");
    newTree->Branch("noise", &noiseValue, "noiseValue/s");
  }

  return newTree;
}

TTree* CreateTreeThresholdScanResult(TString treeName, TString treeTitle)
{
//
// Creates a tree for the Threshold Scan Result
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
// Updated:      09 Jul 2019  Mario Sitta  HIC class added
//

  TTree *newTree = 0;
  newTree = new TTree(treeName.Data(), treeTitle.Data());

  if(newTree) {
    newTree->Branch("hicID", &hicID, "hicID/i");
    newTree->Branch("actID", &actID, "actID/i");
    newTree->Branch("locID", &locID, "locID/I");
    newTree->Branch("startDate", &startDate, "startDate/l");
    newTree->Branch("condVB", &condVB, "condVB/b");
    newTree->Branch("hicClass", &hicClass, "hicClass/B");
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
    newTree->Branch("n8b10bErr", &n8b10bErrors, "n8b10bErrors/I");
    newTree->Branch("corruptEvts", &corruptEvents, "corruptEvents/I");
    newTree->Branch("oversizEvts", &oversizeEvts, "oversizeEvts/I");
    newTree->Branch("timeouts", &timeouts, "timeouts/I");
    newTree->Branch("pixwohits", pixWOHits, "pixWOHits[14]/I");
    newTree->Branch("pixwothrs", pixWOThres, "pixWOThres[14]/I");
    newTree->Branch("hotpix", hotPixels, "hotPixels[14]/I");
    newTree->Branch("avgthres", avrgThres, "avrgThres[14]/F");
    newTree->Branch("rmsthres", thresRMS, "thresRMS[14]/F");
    newTree->Branch("deviation", deviation, "deviation[14]/F");
    newTree->Branch("avgNoise", avrgNoise, "avrgNoise[14]/F");
    newTree->Branch("rmsnoise", noiseRMS, "noiseRMS[14]/F");
    newTree->Branch("reg700Start", reg700Start, "reg700Start[14]/s");
    newTree->Branch("reg700End", reg700End, "reg700End[14]/s");
    newTree->Branch("classificVers", &classificVers, "classificVers/F");
    newTree->Branch("classificThreScan", &classificThreScan, "classificThreScan/I");
//    newTree->Branch("numWorkChips", &numWorkChips, "numWorkChips/F");
  }

  return newTree;
}

void ThresholdScanAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType, std::vector<TChild> children, bool allScans)
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
//          hicType : the HIC type (IB or OB)
//          children: vector of all HIC children
//          allScans: if false analyze only post-tuning scans, if true do all
//
// Outputs:
//
// Return:
//
// Created:      29 Jan 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added, bug fix in chip loop
// Updated:      10 Jul 2019  Mario Sitta  Wafer number and chip position added
//

  hicID = hicid;
  actID = actid;
  locID = actlong.Location.ID;

  // Open Threshold Scan files and fill the tree
  string dataName, resultName;
  unsigned char conds[4] = {100, 200, 103, 203}; // See next method for code meaning

  for (int icond = 0; icond < 4; icond ++) {
    if(conds[icond] < 200 && !allScans) continue;

    const int numchips = ((hicType == HIC_OB) ? NUMCHIPS+1 : NUMCHIPSIB);
 
    for (int ichip = 0; ichip < numchips; ichip++) {
      if(hicType == HIC_IB) chipNum = ichip;
      if(hicType == HIC_OB && ichip == 7) continue;
      if(hicType == HIC_OB && ichip > 7)
	chipNum = ichip - 1;
      else
	chipNum = ichip;

      WaferNumAndPos(hicType, children, chipNum, waferNum, waferPos);

      Int_t code = (conds[icond]/10)*10; // We deliberately divide int's
      Int_t vBB = conds[icond] - code;
      bool nominal = (code == 100);
      if(GetThresholdFileName(actlong, ichip, nominal, vBB, dataName, resultName)) {
        condVB = conds[icond];
        FillThreScanTree(ftree, eospath, dataName);
      }
    }
  }

}

void ThresholdTuneAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType, std::vector<TChild> children)
{
//
// Loops on chips and fills the tree for the threshold tuning 
//
// Inputs:
//          ftree   : the tree to be filled
//          actlong : the activityLong for which the analysis is done
//          hicid   : the HIC id
//          actid   : the activity id
//          eospath : the input file path on EOS
//          hicType : the HIC type (IB or OB)
//          children: vector of all HIC children
//
// Outputs:
//
// Return:
//
// Created:      31 Jan 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added, bug fix in chip loop
// Updated:      10 Jul 2019  Mario Sitta  Wafer number and chip position added
//

  hicID = hicid;
  actID = actid;
  locID = actlong.Location.ID;

  // Open Threshold Tune files and fill the tree
  string dataName, resultName;

  const int numchips = ((hicType == HIC_OB) ? NUMCHIPS+1 : NUMCHIPSIB);
 
  for (int ichip = 0; ichip < numchips; ichip++) {
    if(hicType == HIC_OB && ichip == 7) continue;
    if(hicType == HIC_OB && ichip > 7)
      chipNum = ichip - 1;
    else
      chipNum = ichip;

    if(GetITHRTuneFileName(actlong, ichip, 0, dataName, resultName)) {
      condVB = 100;
      FillThreScanTree(ftree, eospath, dataName);
    }
  }

  for (int ichip = 0; ichip < numchips; ichip++) {
    if(hicType == HIC_OB && ichip == 7) continue;
    if(hicType == HIC_OB && ichip > 7)
      chipNum = ichip - 1;
    else
      chipNum = ichip;

    if(GetVCASNTuneFileName(actlong, ichip, 0, dataName, resultName)) {
      condVB = 200;
      FillThreScanTree(ftree, eospath, dataName);
    }
  }

}

void ThresholdScanResults(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType)
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
// Created:      27 Jan 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
//

  hicID = hicid;
  actID = actid;
  locID = actlong.Location.ID;
  startDate = (ulong)actlong.StartDate;

  // Open Threshold Scan Result files and fill the tree
  string dataName, resultName;

  // Nominal thresholds, 0V BB
  if(GetThresholdFileName(actlong, 0, true, 0, dataName, resultName)) {
    condVB = 100;
    FillThreScanTreeResult(ftree, eospath, resultName, actlong, hicType);
  }

  // Tuned thresholds, 0V BB
  if(GetThresholdFileName(actlong, 0, false, 0, dataName, resultName)) {
    condVB = 200;
    FillThreScanTreeResult(ftree, eospath, resultName, actlong, hicType);
  }

  // Nominal thresholds, 3V BB
  if(GetThresholdFileName(actlong, 0, true, 3, dataName, resultName)) {
    condVB = 103;
    FillThreScanTreeResult(ftree, eospath, resultName, actlong, hicType);
  }

  // Tuned thresholds, 3V BB
  if(GetThresholdFileName(actlong, 0, false, 3, dataName, resultName)) {
    condVB = 203;
    FillThreScanTreeResult(ftree, eospath, resultName, actlong, hicType);
  }

}

Bool_t FillThreScanTree(TTree *tree, string path, string file)
{
//
// Opens the Threshold_FitResults file and fills the tree
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
// Created:      30 Jan 2019  Mario Sitta
// Updated:      27 Mar 2019  Mario Sitta  Fix reading files with , insteda of .
//

  FILE*  infile;
  string fullName;
  Int_t  row, column;
  Float_t thresh, noise, chisq;

  fullName = path + "/" + file;

  infile = fopen(fullName.c_str(),"r");
  if (!infile) {
    printMessage("FillThreScanTree","Warning: cannot open input file",file.c_str());
    return kFALSE;
  }

  char line[50];
//  while(fscanf(infile, "%d %d %f %f %f", &column, &row, &thresh, &noise, &chisq) != EOF){
  while(fgets(line, sizeof(line), infile)){
    SanitizeThresScanInput(line);
    sscanf(line, "%d %d %f %f %f", &column, &row, &thresh, &noise, &chisq);
    rowNum = row;
    colNum = column;
//    thresValue = thresh;
//    noiseValue = noise;
    thresValue = (UShort_t)(thresh*100);
    noiseValue = (UShort_t)(noise*100);
    tree->Fill();
  }
  fclose(infile);

  return kTRUE;
}

Bool_t FillThreScanTreeResult(TTree *tree, string path, string file, ActivityDB::activityLong actlong, const THicType hicType)
{
//
// Opens the ThresholdScanResult file and fills the tree
// (Implementation is a bit ugly since there are many formats
// with a different number of lines and often in different order)
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
// Created:      29 Jan 2019  Mario Sitta  Modelled on Digital Scan routine
// Updated:      05 Feb 2019  Mario Sitta  Bug fix in reading registers
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
//

  FILE*  infile;
  string fullName;
  float value, dummy;
  unsigned int reg, regval;
  int ichip, ivalue;
  char *line = NULL;
  size_t len = 0;

  fullName = path + "/" + file;

  infile = fopen(fullName.c_str(),"r");
  if (!infile) {
    printMessage("FillThreScanTreeResult","Warning: cannot open input file",file.c_str());
    return kFALSE;
  }

  ResetThreScanTreeVariables();

  for (int j=0; j<3; j++) // Skip first three lines
    getline(&line, &len, infile);

  // Scan the file, search for data
  // (file format not unique, lines can be in any order)
  bool chipResulFound = false, regStartFound = false, regEndFound = false;
  while(getline(&line, &len, infile) > 0) {

    if (strlen(line) < 2) continue; // Blank line
    if (strstr(line,"Board registers")) break; // We've finished

    if (strstr(line,"Number of chips")) {
      chipResulFound = true;
      continue;
    }

    if (strstr(line,"Chip registers (start)")) {
      regStartFound = true;
      regEndFound = false;
      continue;
    }
    if (strstr(line,"Chip registers (end)")) {
      regStartFound = false;
      regEndFound = true;
      continue;
    }

    if(regStartFound || regEndFound) goto chipRegisters; // OMG, a goto!!!
    if(chipResulFound) goto singleChipResults; // OOMG, a second goto!!!

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
      continue;
    }

    if (strstr(line,"Analogue voltage (") && strstr(line,"end")) {
      sscanf(line, "  Analogue voltage (end) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
        chipAnalVoltEnd[ichip-1] = value;
      else
        chipAnalVoltEnd[ichip] = value;
      continue;
    }

    if (strstr(line,"Digital voltage (") && strstr(line,"start")) {
      sscanf(line, "  Digital voltage (start) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
        chipDigiVoltStart[ichip-1] = value;
      else
        chipDigiVoltStart[ichip] = value;
      continue;
    }

    if (strstr(line,"Digital voltage (") && strstr(line,"end")) {
      sscanf(line, "  Digital voltage (end) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
        chipDigiVoltEnd[ichip-1] = value;
      else
        chipDigiVoltEnd[ichip] = value;
      continue;
    }

    if (strstr(line,"Temperature (") && strstr(line,"start")) {
      sscanf(line, "  Temperature (start) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
        chipTempStart[ichip-1] = value;
      else
        chipTempStart[ichip] = value;
      continue;
    }

    if (strstr(line,"Temperature (") && strstr(line,"end")) {
      sscanf(line, "  Temperature (end) on chip %d: %f", &ichip, &value);
      if(hicType == HIC_OB && ichip > 7)
        chipTempEnd[ichip-1] = value;
      else
        chipTempEnd[ichip] = value;
      continue;
    }

    // Summary data
    if (strstr(line, "8b10b errors")) {
      sscanf(line, "8b10b errors:   %d", &n8b10bErrors);
      continue;
    }

    if (strstr(line, "Corrupt events")) {
      sscanf(line, "Corrupt events: %d", &corruptEvents);
      continue;
    }

    if (strstr(line, "Oversized events")) {
      sscanf(line, "Oversized events:       %d", &oversizeEvts);
      continue;
    }

    if (strstr(line, "Timeouts")) {
      sscanf(line, "Timeouts:       %d", &timeouts);
      continue;
    }

    // Single chip results
  singleChipResults:
    if (strstr(line, "Result chip")) {
      sscanf(line, "Result chip %d:", &ichip);
      if(hicType == HIC_OB && ichip > 7) ichip--;
      continue;
    }

    if (strstr(line, "Pixels without hits")) {
      sscanf(line, "Pixels without hits:      %d", &ivalue);
      pixWOHits[ichip] = ivalue;
      continue;
    }

    if (strstr(line, "Pixels without threshold")) {
      sscanf(line, "Pixels without threshold: %d", &ivalue);
      pixWOThres[ichip] = ivalue;
      continue;
    }

    if (strstr(line, "Hot pixels")) {
      sscanf(line, "Hot pixels:               %d", &ivalue);
      hotPixels[ichip] = ivalue;
      continue;
    }

    if (strstr(line, "Av. Threshold")) {
      sscanf(line, "Av. Threshold: %f", &value);
      avrgThres[ichip] = value;
      continue;
    }

    if (strstr(line, "Threshold RMS")) {
      sscanf(line, "Threshold RMS: %f", &value);
      thresRMS[ichip] = value;
      continue;
    }

    if (strstr(line, "Deviation")) {
      sscanf(line, "Deviation:     %f", &value);
      deviation[ichip] = value;
      continue;
    }

    if (strstr(line, "Av. Noise")) {
      sscanf(line, "Av. Noise:     %f", &value);
      avrgNoise[ichip] = value;
      continue;
    }

    if (strstr(line, "Noise RMS")) {
      sscanf(line, "Noise RMS:     %f", &value);
      noiseRMS[ichip] = value;
      continue;
    }

    // Registers data
  chipRegisters:
    if (strstr(line,"# Chip ID"))
      continue;

    sscanf(line, "%d        0x%x        0x%x", &ichip, &reg, &regval);

    if(reg == 0x700) {
      ichip &= 0x0f; // ichip is the lower 4 bits
      if(regStartFound) {
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

    if( (condVB == 100 || condVB == 200) &&
	actpar.Type.Parameter.Name.compare("Classification Threshold Scan 0.0 V") == 0) {
      classificThreScan = actpar.Value;
      continue;
    }

    if( (condVB == 103 || condVB == 203) &&
	actpar.Type.Parameter.Name.compare("Classification Threshold Scan 3.0 V") == 0) {
      classificThreScan = actpar.Value;
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

Bool_t FindActivityInThreScanTree(TTree* listree, const UInt_t hicid, const UInt_t actid, const UShort_t mask)
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
// Created:      28 Nov 2018  Mario Sitta
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

TTree* ReadHicActListTreeTS(TFile *rootfile)
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
// Updated:      18 Jan 2019  Mario Sitta
//

  TTree *newtree = 0;
  newtree = (TTree*)rootfile->Get("actFastListTree");

  if(newtree) {
    newtree->SetBranchAddress("hicID", &hicID);
    newtree->SetBranchAddress("actID", &actID);
    newtree->SetBranchAddress("actMask", &actMask);
    newtree->SetBranchAddress("actOffs", &testOffset);
    newtree->SetBranchAddress("actTunOff", &testTunOffset);
    newtree->SetBranchAddress("actResOff", &testResOffset);
  }

  return newtree;
}

TTree* ReadThreScanTree(TString treename, TFile *rootfile)
{
//
// Reads a tree for Threshold Scan from file
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
// Created:      29 Jan 2019  Mario Sitta
// Updated:      05 Jul 2019  Mario Sitta  Chip Wafer and position added
// Updated:      09 Jul 2019  Mario Sitta  HIC class added
//

  TTree *newtree = 0;
  newtree = (TTree*)rootfile->Get(treename.Data());

  if(newtree) {
    newtree->SetBranchAddress(   "hicID",  &hicID);
    newtree->SetBranchAddress(   "actID",  &actID);
    newtree->SetBranchAddress(   "locID",  &locID);
    newtree->SetBranchAddress(  "condVB", &condVB);
    newtree->SetBranchAddress("hicClass", &hicClass);
    newtree->SetBranchAddress( "chipNum",&chipNum);
    newtree->SetBranchAddress("waferNum",&waferNum);
    newtree->SetBranchAddress("waferPos",&waferPos);
    newtree->SetBranchAddress(  "colNum", &colNum);
    newtree->SetBranchAddress(  "rowNum", &rowNum);
    newtree->SetBranchAddress(  "thresh", &thresValue);
    newtree->SetBranchAddress(   "noise", &noiseValue);
  }

  return newtree;
}

TTree* ReadThreScanTreeResult(TString treename, TFile *rootfile)
{
//
// Reads a tree for Threshold Scan from file
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
// Created:      29 Nov 2019  Mario Sitta
// Updated:      09 Jul 2019  Mario Sitta  HIC class added
//

  TTree *newtree = 0;
  newtree = (TTree*)rootfile->Get(treename.Data());

  if(newtree) {
    newtree->SetBranchAddress(            "hicID", &hicID);
    newtree->SetBranchAddress(            "actID", &actID);
    newtree->SetBranchAddress(            "locID", &locID);
    newtree->SetBranchAddress(        "startDate", &startDate);
    newtree->SetBranchAddress(           "condVB", &condVB);
    newtree->SetBranchAddress(         "hicClass", &hicClass);
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
    newtree->SetBranchAddress("chipAnalVoltStart", chipAnalVoltStart);
    newtree->SetBranchAddress(  "chipAnalVoltEnd", chipAnalVoltEnd);
    newtree->SetBranchAddress("chipDigiVoltStart", chipDigiVoltStart);
    newtree->SetBranchAddress(  "chipDigiVoltEnd", chipDigiVoltEnd);
    newtree->SetBranchAddress(    "chipTempStart", chipTempStart);
    newtree->SetBranchAddress(      "chipTempEnd", chipTempEnd);
    newtree->SetBranchAddress(        "n8b10bErr", &n8b10bErrors);
    newtree->SetBranchAddress(      "corruptEvts", &corruptEvents);
    newtree->SetBranchAddress(      "oversizEvts", &oversizeEvts);
    newtree->SetBranchAddress(         "timeouts", &timeouts);
    newtree->SetBranchAddress(        "pixwohits", pixWOHits);
    newtree->SetBranchAddress(        "pixwothrs", pixWOThres);
    newtree->SetBranchAddress(           "hotpix", hotPixels);
    newtree->SetBranchAddress(         "avgthres", avrgThres);
    newtree->SetBranchAddress(         "rmsthres", thresRMS);
    newtree->SetBranchAddress(        "deviation", deviation);
    newtree->SetBranchAddress(         "avgNoise", avrgNoise);
    newtree->SetBranchAddress(         "rmsnoise", noiseRMS);
    newtree->SetBranchAddress(      "reg700Start", reg700Start);
    newtree->SetBranchAddress(        "reg700End", reg700End);
    newtree->SetBranchAddress(    "classificVers", &classificVers);
    newtree->SetBranchAddress("classificThreScan", &classificThreScan);
//    newtree->SetBranchAddress(     "numWorkChips", &numWorkChips);
  }

  return newtree;
}

void ResetThreScanTreeVariables(void)
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
// Created:      09 Nov 2018  Mario Sitta
// Updated:      22 Nov 2018  Mario Sitta
// Updated:      05 Jul 2019  Mario Sitta  Chip Wafer and position added
// Updated:      09 Jul 2019  Mario Sitta  HIC class added
//

  hicClass = 0;
  waferNum = 0;
  waferPos = 0;
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
  n8b10bErrors = 0;
  corruptEvents = 0;
  oversizeEvts = 0;
  timeouts = 0;
  for (int i = 0; i < NUMCHIPS; i++) {
    chipAnalVoltStart[i] = 0;
    chipAnalVoltEnd[i] = 0;
    chipDigiVoltStart[i] = 0;
    chipDigiVoltEnd[i] = 0;
    chipTempStart[i] = 0;
    chipTempEnd[i] = 0;
    pixWOHits[i] = 0;
    pixWOThres[i] = 0;
    hotPixels[i] = 0;
    avrgThres[i] = 0;
    thresRMS[i] = 0;
    deviation[i] = 0;
    avrgNoise[i] = 0;
    noiseRMS[i] = 0;
    reg700Start[i] = 0;
    reg700End[i] = 0;
  }
  classificVers = 0;
  classificThreScan = 0;
//  numWorkChips = 0;
}

void SanitizeThresScanInput(char *line)
{
//
// Fixes the problem of some files on the Bari site
// by replacing the comma with the dot as separator
// between integer part and decimal part or float numbers
//
// Inputs:
//          line : the input line before corection
//
// Outputs:
//          line : the input line after corection
//
// Return:
//          a pointer to the created/read tree
//
// Created:      26 Mar 2019  Mario Sitta
//

  const Int_t length = strlen(line);

  for(Int_t j = 0; j < length; j++)
    if(line[j] == ',') line[j] = '.';
}

TTree* SetupHicActListTreeTS(TFile *rootfile)
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
    newtree = CreateHicActListTreeTS();
  else
    newtree = ReadHicActListTreeTS(rootfile);

  return newtree;
}

TTree* SetupThreScanTree(TString treename, TString treetitle, TFile *rootfile)
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
    newtree = CreateTreeThresholdScan(treename,treetitle);
  else
    newtree = ReadThreScanTree(treename, rootfile);

  return newtree;
}

TTree* SetupThreScanTreeResult(TString treename, TString treetitle, TFile *rootfile)
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
    newtree = CreateTreeThresholdScanResult(treename,treetitle);
  else
    newtree = ReadThreScanTreeResult(treename, rootfile);

  return newtree;
}

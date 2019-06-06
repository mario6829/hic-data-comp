#include "dctrltestlib.h"
#include "utillib.h"
#include "menulib.h"
#include "treevariables.h"

// Local tree variables
UChar_t  driverSet;
Float_t  peak2peakP;
Float_t  peak2peakN;
Float_t  amplitudeP;
Float_t  amplitudeN;
Double_t riseTimeP;
Double_t riseTimeN;
Double_t fallTimeP;
Double_t fallTimeN;
Float_t  worsMaxAmpl;
Float_t  worsSlope;
Float_t  worsSlopeRat;
Float_t  worsChiSq;
Float_t  worsChiSqRat;
Float_t  worsCorrel;
Double_t worsRiseTim;
Double_t worsFallTim;
Float_t  slopePos[NUMCHIPS];
Float_t  intercPos[NUMCHIPS];
Float_t  chisqPos[NUMCHIPS];
Float_t  correlPos[NUMCHIPS];
Float_t  maxAmpPos[NUMCHIPS];
Double_t maxRisePos[NUMCHIPS];
Double_t maxFallPos[NUMCHIPS];
Float_t  slopeNeg[NUMCHIPS];
Float_t  intercNeg[NUMCHIPS];
Float_t  chisqNeg[NUMCHIPS];
Float_t  correlNeg[NUMCHIPS];
Float_t  maxAmpNeg[NUMCHIPS];
Double_t maxRiseNeg[NUMCHIPS];
Double_t maxFallNeg[NUMCHIPS];
Float_t  classificDctrlTest;



void analyzeAllDCTRLTests(std::vector<ComponentDB::componentShort> componentList, AlpideDB *db, const THicType hicType)
{
//
// Steering routine to analyze the data of DCTRL Test for all HICs
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
// Created:      09 Feb 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
// Updated:      07 Mar 2019  Mario Sitta  HIC position added
// Updated:      08 Mar 2019  Mario Sitta  Flag ML/OL staves
// Updated:      08 Mar 2019  Mario Sitta  Stave Reception Test added
//

  // We need to define here the TTree's for the existing ROOT file
  TTree *oldHicQualTree = 0, *oldHicRecpTree = 0, *oldHicHSTree = 0, *oldHicStaveQualTree = 0, *oldHicStaveRecpTree = 0;;
  TTree *oldHicQualResTree = 0, *oldHicRecpResTree = 0, *oldHicHSResTree = 0, *oldHicStaveQualResTree = 0, *oldHicStaveRecpResTree = 0;
  TTree *oldActFastListTree = 0;

  // Should never happen (the caller should have created it for us)
  if (!db) {
    printMessage("analyzeAllDCTRLTests","Error: the DataBase was not opened");
    f12ToExit();
    return;
  }

  ActivityDB *activityDB = new ActivityDB(db);

  // Check whether the ROOT file already exists
  // If yes, ask the user whether to use it or redo a new one
  TString rootFileName;
  if (hicType == HIC_IB)
    rootFileName = "IBHIC_DCTRLTest_AllHICs.root";
  else
    rootFileName = "OBHIC_DCTRLTest_AllHICs.root";

  redoFromStart = kTRUE;
  if(CheckRootFileExists(rootFileName)) {
    if(AskUserRedoScan() == 2) { // User chose to re-use existing tree
      redoFromStart = kFALSE;
      TString oldRootFileName;
      if(!RenameExistingRootFile(rootFileName, "_old", oldRootFileName)) {
        printMessage("\nanalyzeAllDCTRLTests","Error: error renaming existing ROOT file");
        f12ToExit();
        return;
      }

      TFile *oldDctrltestFile = OpenRootFile(oldRootFileName);
      if(!oldDctrltestFile) {
        printMessage("\nanalyzeAllDigitalScans","Error: error opening existing ROOT file");
        f12ToExit();
        return;
      }

      oldHicQualTree = ReadDctrlTestTree("hicQualTree",oldDctrltestFile);
      oldHicRecpTree = ReadDctrlTestTree("hicRecpTree",oldDctrltestFile);
      oldHicHSTree = ReadDctrlTestTree("hicHSTree",oldDctrltestFile);
      oldHicStaveQualTree = ReadDctrlTestTree("hicStaveQualTree",oldDctrltestFile);
      oldHicStaveRecpTree = ReadDctrlTestTree("hicStaveRecpTree",oldDctrltestFile);

      oldHicQualResTree = ReadDctrlTestTreeResult("hicQualResTree",oldDctrltestFile);
      oldHicRecpResTree = ReadDctrlTestTreeResult("hicRecpResTree",oldDctrltestFile);
      oldHicHSResTree = ReadDctrlTestTreeResult("hicHSResTree",oldDctrltestFile);
      oldHicStaveQualResTree = ReadDctrlTestTreeResult("hicStaveQualResTree",oldDctrltestFile);
      oldHicStaveRecpResTree = ReadDctrlTestTreeResult("hicStaveRecpResTree",oldDctrltestFile);

      oldActFastListTree = ReadHicActListTreeDT(oldDctrltestFile);

      if(!oldHicQualTree || !oldHicRecpTree || !oldHicHSTree || !oldHicStaveQualTree || !oldHicStaveRecpTree ||
         !oldHicQualResTree || !oldHicRecpResTree || !oldHicHSResTree || !oldHicStaveQualResTree || !oldHicStaveRecpResTree ||
         !oldActFastListTree) {
        printMessage("\nanalyzeAllDCTRLTests","Error: error reading trees from existing ROOT file");
        f12ToExit();
        return;
      }
    } // if(AskUserRedoScan())
  } // if(CheckRootFileExists())

  // Open the ROOT file
  TFile *newDctrltestFile = OpenRootFile(rootFileName, kTRUE);

  if(!newDctrltestFile) {
    printMessage("\nanalyzeAllDCTRLTests","Error: error opening new ROOT file");
    f12ToExit();
    return;
  }

  // Create or read the trees
  TTree *hicQualTree = CreateTreeDctrlTest("hicQualTree","HicQualificationTest");
  TTree *hicRecpTree = CreateTreeDctrlTest("hicRecpTree","HicReceptionTest");
  TTree *hicHSTree = CreateTreeDctrlTest("hicHSTree","HicHalfStaveTest");
  TTree *hicStaveQualTree = CreateTreeDctrlTest("hicStaveQualTree","HicStaveQualTest");
  TTree *hicStaveRecpTree = CreateTreeDctrlTest("hicStaveRecpTree","HicStaveQualTest");

  TTree *hicQualResTree = CreateTreeDctrlTestResult("hicQualResTree","HicQualificationTestResults");
  TTree *hicRecpResTree = CreateTreeDctrlTestResult("hicRecpResTree","HicReceptionTestResults");
  TTree *hicHSResTree = CreateTreeDctrlTestResult("hicHSResTree","HicHalfStaveTestResults");
  TTree *hicStaveQualResTree = CreateTreeDctrlTestResult("hicStaveQualResTree","HicStaveQualTestResults");
  TTree *hicStaveRecpResTree = CreateTreeDctrlTestResult("hicStaveRecpResTree","HicStaveQualTestResults");

  TTree *actFastListTree = CreateHicActListTreeDT();

  // Loop on all components
  int totHICAnal = 0, totActAnal = 0;
  std::vector<ComponentDB::compActivity> tests;
  std::vector<ComponentDB::componentShort>::iterator iComp;
  for (iComp = componentList.begin(); iComp != componentList.end(); iComp++) {
    ComponentDB::componentShort comp = *iComp;
    DbGetAllTests (db, comp.ID, tests, STDctrl, true);

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
	if(FindActivityInDctrlTestTree(oldActFastListTree, comp.ID, act.ID, actMask)) {
          printMessage("\nanalyzeAllDCTRLTests", "Activity already in file, copying trees ", actLong.Name.c_str());
          CopyDctrlTestOldToNew(comp.ID, act.ID, testree, resultree, oldtestree, oldresultree);
          actFastListTree->Fill();
          continue;
        }

      string eosPath = FindEOSPath(actLong, hicType);
      if(eosPath.length() == 0) { // No valid path found on EOS
        string hicAct = actLong.Name + " " + actLong.Type.Name;
        printMessage("\nanalyzeAllDCTRLTests", "EOS for this activity does not exists", hicAct.c_str());
        continue;
      }

      // Fill the tree for all chips
      testOffset = testree->GetEntries();
      testResOffset = resultree->GetEntries();

      hicPosition = DbGetPosition(db, comp.ID);

      DctrlTestAllChips(testree, actLong, comp.ID, act.ID, eosPath);
      DctrlTestResults(resultree, actLong, comp.ID, act.ID, eosPath, hicType);

      Long64_t prevTestOffset = testree->GetEntries();
      Long64_t prevTestResOffset = resultree->GetEntries();

      if(testOffset == prevTestOffset || testResOffset == prevTestResOffset)
        printMessage("\nanalyzeAllDCTRLTests", "Trees not filled for activity ", actLong.Name.c_str());
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
  CloseRootFile(newDctrltestFile);

#ifdef USENCURSES
  mvprintw(LINES-4, 0, "\n ROOT file %s filled with %d activities\n", rootFileName.Data(), totActAnal);
#else
  printf("\n\n ROOT file %s filled with %d activities\n", rootFileName.Data(), totActAnal);
#endif
  f12ToExit();
}

void analyzeDCTRLTest(const int hicid, const ComponentDB::compActivity act, AlpideDB *db, const THicType hicType)
{
//
// Steering routine to analyze DCTRL Test data for one HIC
//
// Inputs:
//          hicid   : the HIC id
//          act     : the HIC activity whose Dctrltest will be analyzed
//          db      : a pointer to the Alpide DB
//          hicType : the HIC type (IB or OB)
//
// Outputs:
//
// Return:
//
// Created:      07 Feb 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
// Updated:      07 Mar 2019  Mario Sitta  HIC position added
// Updated:      08 Mar 2019  Mario Sitta  Flag ML/OL staves
// Updated:      06 Jun 2019  Mario Sitta  Get rid of timestamp from act name
//

  // Should never happen (the caller should have created it for us)
  if (!db) {
    printMessage("analyzeDCTRLTest","Error: the DataBase was not opened");
    f12ToExit();
    return;
  }

  ActivityDB *activityDB = new ActivityDB(db);

  // Get the proper path to the DCTRL Test result files
  ActivityDB::activityLong actLong;
  activityDB->Read(act.ID, &actLong);

  string eosPath = FindEOSPath(actLong, hicType);
  if (eosPath == "") {
    printMessage("\nanalyzeDCTRLTest","Error: no valid EOS path found");
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
  rootFileName += "_DCTRLTest.root";

  TFile *dctrltestFile = OpenRootFile(rootFileName, kTRUE);
  if (!dctrltestFile) {
    printMessage("\nanalyzeDCTRLTest","Error: error opening the ROOT file");
    f12ToExit();
    return;
  }

  // Create the trees
  TTree *dctrltestTree = CreateTreeDctrlTest("dctrltestTree","DctrlTestTree");
  if (!dctrltestTree) {
    printMessage("\nanalyzeDCTRLTest","Error: error creating the ROOT tree");
    f12ToExit();
    return;
  }

  TTree *dctrlresulTree = CreateTreeDctrlTestResult("dctrlresulTree","DctrlTestResulTree");
  if (!dctrlresulTree) {
    printMessage("\nanalyzeDCTRLTest","Error: error creating the ROOT tree");
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
  DctrlTestAllChips(dctrltestTree, actLong, hicid, act.ID, eosPath);
  DctrlTestResults(dctrlresulTree, actLong, hicid, act.ID, eosPath, hicType);

  // Close the ROOT file and exit
  dctrltestTree->Write();
  dctrlresulTree->Write();
  CloseRootFile(dctrltestFile);

#ifdef USENCURSES
  printw("\n ROOT file %s filled\n", rootFileName.c_str());
#else
  printf("\n ROOT file %s filled\n", rootFileName.c_str());
#endif
  f12ToExit();

}

void CopyDctrlTestOldToNew(const UInt_t hicid, const UInt_t actid,
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

  // Save current values (they were filled by FindActivityInDctrlTestTree
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

TTree* CreateHicActListTreeDT(void)
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

TTree* CreateTreeDctrlTest(TString treeName, TString treeTitle)
{
//
// Creates a tree for the DCTRL Test
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
// Created:      07 Feb 2019  Mario Sitta
// Updated:      07 Mar 2019  Mario Sitta  HIC position added
// Updated:      08 Mar 2019  Mario Sitta  Flag ML/OL staves
//

  TTree *newTree = 0;
  newTree = new TTree(treeName.Data(), treeTitle.Data());

  if(newTree) {
    newTree->Branch("hicID", &hicID, "hicID/i");
    newTree->Branch("actID", &actID, "actID/i");
    newTree->Branch("locID", &locID, "locID/I");
    newTree->Branch("hicPos", &hicPosition, "hicPosition/B");
    newTree->Branch("staveOLML", &staveOLML, "staveOLML/b");
    newTree->Branch("chipNum", &chipNum, "chipNum/b");
    newTree->Branch("driverSet", &driverSet, "driverSet/b");
    newTree->Branch("pk2pkP", &peak2peakP, "peak2peakP/F");
    newTree->Branch("pk2pkN", &peak2peakN, "peak2peakN/F");
    newTree->Branch("ampliP", &amplitudeP, "amplitudeP/F");
    newTree->Branch("ampliN", &amplitudeN, "amplitudeN/F");
    newTree->Branch("risetimeP", &riseTimeP, "riseTimeP/D");
    newTree->Branch("risetimeN", &riseTimeN, "riseTimeN/D");
    newTree->Branch("falltimeP", &fallTimeP, "fallTimeP/D");
    newTree->Branch("falltimeN", &fallTimeN, "fallTimeN/D");
  }

  return newTree;
}

TTree* CreateTreeDctrlTestResult(TString treeName, TString treeTitle)
{
//
// Creates a tree for the DCTRL Test Result
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
// Created:      07 Feb 2019  Mario Sitta
// Updated:      07 Mar 2019  Mario Sitta  HIC position added
// Updated:      08 Mar 2019  Mario Sitta  Flag ML/OL staves
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
    newTree->Branch("worsMaxAmpl", &worsMaxAmpl, "worsMaxAmpl/F");
    newTree->Branch("worsSlope", &worsSlope, "worsSlope/F");
    newTree->Branch("worsSlopeRat", &worsSlopeRat, "worsSlopeRat/F");
    newTree->Branch("worsChiSq", &worsChiSq, "worsChiSq/F");
    newTree->Branch("worsChiSqRat", &worsChiSqRat, "worsChiSqRat/F");
    newTree->Branch("worsCorrel", &worsCorrel, "worsCorrel/F");
    newTree->Branch("worsRiseTim", &worsRiseTim, "worsRiseTim/D");
    newTree->Branch("worsFallTim", &worsFallTim, "worsFallTim/D");
    newTree->Branch("slopePos", slopePos, "slopePos[14]/F");
    newTree->Branch("intercPos", intercPos, "intercPos[14]/F");
    newTree->Branch("chisqPos", chisqPos, "chisqPos[14]/F");
    newTree->Branch("correlPos", correlPos, "correlPos[14]/F");
    newTree->Branch("maxAmpPos", maxAmpPos, "maxAmpPos[14]/F");
    newTree->Branch("maxRisePos", maxRisePos, "maxRisePos[14]/D");
    newTree->Branch("maxFallPos", maxFallPos, "maxFallPos[14]/D");
    newTree->Branch("slopeNeg", slopeNeg, "slopeNeg[14]/F");
    newTree->Branch("intercNeg", intercNeg, "intercNeg[14]/F");
    newTree->Branch("chisqNeg", chisqNeg, "chisqNeg[14]/F");
    newTree->Branch("correlNeg", correlNeg, "correlNeg[14]/F");
    newTree->Branch("maxAmpNeg", maxAmpNeg, "maxAmpNeg[14]/F");
    newTree->Branch("maxRiseNeg", maxRiseNeg, "maxRiseNeg[14]/D");
    newTree->Branch("maxFallNeg", maxFallNeg, "maxFallNeg[14]/D");
    newTree->Branch("reg700Start", reg700Start, "reg700Start[14]/s");
    newTree->Branch("reg700End", reg700End, "reg700End[14]/s");
    newTree->Branch("classificVers", &classificVers, "classificVers/F");
    newTree->Branch("classifDctrlTest", &classificDctrlTest, "classificDctrlTest/F");
  }

  return newTree;
}

void DctrlTestAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath)
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
// Created:      07 Feb 2019  Mario Sitta
//

  hicID = hicid;
  actID = actid;
  locID = actlong.Location.ID;

  // Open I-V Current file and fill the tree
  string dataName, resultName;

  if(GetDctrlFileName(actlong, dataName, resultName)) {
    FillDctrlTestTree(ftree, eospath, dataName);
  }

}

void DctrlTestResults(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType)
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

  if(GetDctrlFileName(actlong, dataName, resultName)) {
    FillDctrlTestTreeResult(ftree, eospath, resultName, actlong, hicType);
  }

}

Bool_t FillDctrlTestTree(TTree *tree, string path, string file)
{
//
// Opens the DCTRL Test file and fills the tree
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
// Created:      07 Feb 2019  Mario Sitta
//

  FILE*  infile;
  string fullName;
  int    ichip, drivset;
  float  pk2pkpos, pk2pkneg, amplpos, amplneg;
  double risetpos, risetneg, falltpos, falltneg;

  fullName = path + "/" + file;

  infile = fopen(fullName.c_str(),"r");
  if (!infile) {
    printMessage("FillDctrlTestTree","Warning: cannot open input file",file.c_str());
    return kFALSE;
  }

  while(fscanf(infile, "%d %d %f %f %f %f %lf %lf %lf %lf",
	       &ichip, &drivset, &pk2pkpos, &pk2pkneg, &amplpos, &amplneg,
	       &risetpos, &risetneg, &falltpos, &falltneg) != EOF){
    chipNum = ichip;
    driverSet = drivset;
    peak2peakP = pk2pkpos;
    peak2peakN = pk2pkneg;
    amplitudeP = amplpos;
    amplitudeN = amplneg;
    riseTimeP = risetpos;
    riseTimeN = risetneg;
    fallTimeP = falltpos;
    fallTimeN = falltneg;
    tree->Fill();
  }
  fclose(infile);

  return kTRUE;
}

Bool_t FillDctrlTestTreeResult(TTree *tree, string path, string file, ActivityDB::activityLong actlong, const THicType hicType)
{
//
// Opens the DctrlScanResult file and fills the tree
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
// Created:      08 Feb 2019  Mario Sitta
// Updated:      26 Feb 2019  Mario Sitta  HIC type added
//

  FILE*  infile;
  string fullName;
  double dvalue;
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

  ResetDctrlTestTreeVariables();

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

    // Specific DCTRL Test data and single chip results
  singleChipResults:
    if (strstr(line, "positive polarity") || strstr(line, "negative polarity"))
      continue;

    if (strstr(line, "Result chip")) {
      sscanf(line, "Result chip %d:", &ichip);
      if(hicType == HIC_OB && ichip > 7) ichip--;
      continue;
    }

    if (strstr(line, "Worst maximum amplitude")) {
      sscanf(line, "Worst maximum amplitude: %f", &worsMaxAmpl);
      continue;
    }

    if (strstr(line, "Worst slope")) {
      sscanf(line, "Worst slope:             %f", &worsSlope);
      getline(&line, &len, infile); // Done here 'cause 2 lines with same text
      sscanf(line, "    ratio to previous:   %f", &worsSlopeRat);
      continue;
    }

    if (strstr(line, "Worst chi square")) {
      sscanf(line, "Worst chi square:        %f", &worsChiSq);
      getline(&line, &len, infile); // Done here 'cause 2 lines with same text
      sscanf(line, "    ratio to previous:   %f", &worsChiSqRat);
      continue;
    }

    if (strstr(line, "Worst correlation")) {
      sscanf(line, "Worst correlation:       %f", &worsCorrel);
      continue;
    }

    if (strstr(line, "Worst rise time")) {
      sscanf(line, "Worst rise time:         %lf", &worsRiseTim);
      continue;
    }

    if (strstr(line, "Worst fall time")) {
      sscanf(line, "Worst fall time:         %lf", &worsFallTim);
      continue;
    }

    if (strstr(line, "Slope p")) {
      sscanf(line, "Slope p: %f", &value);
      slopePos[ichip] = value;
      continue;
    }

    if (strstr(line, "Intercept p")) {
      sscanf(line, "Intercept p: %f", &value);
      intercPos[ichip] = value;
      continue;
    }

    if (strstr(line, "Chi sq p")) {
      sscanf(line, "Chi sq p: %f", &value);
      chisqPos[ichip] = value;
      continue;
    }

    if (strstr(line, "Correlation coeff p")) {
      sscanf(line, "Correlation coeff p: %f", &value);
      correlPos[ichip] = value;
      continue;
    }

    if (strstr(line, "Max. amplitude p")) {
      sscanf(line, "Max. amplitude p: %f", &value);
      maxAmpPos[ichip] = value;
      continue;
    }

    if (strstr(line, "Max. rise time p")) {
      sscanf(line, "Max. rise time p: %lf", &dvalue);
      maxRisePos[ichip] = dvalue;
      continue;
    }

    if (strstr(line, "Max. fall time p")) {
      sscanf(line, "Max. fall time p: %lf", &dvalue);
      maxFallPos[ichip] = dvalue;
      continue;
    }

    if (strstr(line, "Slope n")) {
      sscanf(line, "Slope n: %f", &value);
      slopeNeg[ichip] = value;
      continue;
    }

    if (strstr(line, "Intercept n")) {
      sscanf(line, "Intercept n: %f", &value);
      intercNeg[ichip] = value;
      continue;
    }

    if (strstr(line, "Chi sq n")) {
      sscanf(line, "Chi sq n: %f", &value);
      chisqNeg[ichip] = value;
      continue;
    }

    if (strstr(line, "Correlation coeff n")) {
      sscanf(line, "Correlation coeff n: %f", &value);
      correlNeg[ichip] = value;
      continue;
    }

    if (strstr(line, "Max. amplitude n")) {
      sscanf(line, "Max. amplitude n: %f", &value);
      maxAmpNeg[ichip] = value;
      continue;
    }

    if (strstr(line, "Max. rise time n")) {
      sscanf(line, "Max. rise time n: %lf", &dvalue);
      maxRiseNeg[ichip] = dvalue;
      continue;
    }

    if (strstr(line, "Max. fall time n")) {
      sscanf(line, "Max. fall time n: %lf", &dvalue);
      maxFallNeg[ichip] = dvalue;
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

    if(actpar.Type.Parameter.Name.compare("Classification Dctrl Measurement") == 0) {
      classificDctrlTest = actpar.Value;
      continue;
    }

  }

  // Fill the tree, close the file and return
  tree->Fill();

  return kTRUE;
}

Bool_t FindActivityInDctrlTestTree(TTree* listree, const UInt_t hicid, const UInt_t actid, const UShort_t mask)
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
// Created:      09 Feb 2019  Mario Sitta
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

TTree* ReadHicActListTreeDT(TFile *rootfile)
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

TTree* ReadDctrlTestTree(TString treename, TFile *rootfile)
{
//
// Reads a tree for DCTRL Test from file
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
// Updated:      07 Mar 2019  Mario Sitta  HIC position added
// Updated:      08 Mar 2019  Mario Sitta  Flag ML/OL staves
//

  TTree *newtree = 0;
  newtree = (TTree*)rootfile->Get(treename.Data());

  if(newtree) {
    newtree->SetBranchAddress("hicID", &hicID);
    newtree->SetBranchAddress("actID", &actID);
    newtree->SetBranchAddress("locID", &locID);
    newtree->SetBranchAddress("hicPos", &hicPosition);
    newtree->SetBranchAddress("staveOLML", &staveOLML);
    newtree->SetBranchAddress("chipNum", &chipNum);
    newtree->SetBranchAddress("driverSet", &driverSet);
    newtree->SetBranchAddress("pk2pkP", &peak2peakP);
    newtree->SetBranchAddress("pk2pkN", &peak2peakN);
    newtree->SetBranchAddress("ampliP", &amplitudeP);
    newtree->SetBranchAddress("ampliN", &amplitudeN);
    newtree->SetBranchAddress("risetimeP", &riseTimeP);
    newtree->SetBranchAddress("risetimeN", &riseTimeN);
    newtree->SetBranchAddress("falltimeP", &fallTimeP);
    newtree->SetBranchAddress("falltimeN", &fallTimeN);
  }

  return newtree;
}

TTree* ReadDctrlTestTreeResult(TString treename, TFile *rootfile)
{
//
// Reads a tree for DCTRL Test from file
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
// Updated:      07 Mar 2019  Mario Sitta  HIC position added
// Updated:      08 Mar 2019  Mario Sitta  Flag ML/OL staves
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
    newtree->SetBranchAddress(        "tempStart", &tempStart);
    newtree->SetBranchAddress(          "tempEnd", &tempEnd);
    newtree->SetBranchAddress("chipAnalVoltStart",  chipAnalVoltStart);
    newtree->SetBranchAddress(  "chipAnalVoltEnd",  chipAnalVoltEnd);
    newtree->SetBranchAddress("chipDigiVoltStart", chipDigiVoltStart);
    newtree->SetBranchAddress(  "chipDigiVoltEnd", chipDigiVoltEnd);
    newtree->SetBranchAddress(    "chipTempStart",  chipTempStart);
    newtree->SetBranchAddress(      "chipTempEnd",  chipTempEnd);
    newtree->SetBranchAddress(      "worsMaxAmpl", &worsMaxAmpl);
    newtree->SetBranchAddress(        "worsSlope", &worsSlope);
    newtree->SetBranchAddress(     "worsSlopeRat", &worsSlopeRat);
    newtree->SetBranchAddress(        "worsChiSq", &worsChiSq);
    newtree->SetBranchAddress(     "worsChiSqRat", &worsChiSqRat);
    newtree->SetBranchAddress(       "worsCorrel", &worsCorrel);
    newtree->SetBranchAddress(      "worsRiseTim", &worsRiseTim);
    newtree->SetBranchAddress(      "worsFallTim", &worsFallTim);
    newtree->SetBranchAddress(         "slopePos",  slopePos);
    newtree->SetBranchAddress(        "intercPos",  intercPos);
    newtree->SetBranchAddress(         "chisqPos",  chisqPos);
    newtree->SetBranchAddress(        "correlPos",  correlPos);
    newtree->SetBranchAddress(        "maxAmpPos",  maxAmpPos);
    newtree->SetBranchAddress(       "maxRisePos",  maxRisePos);
    newtree->SetBranchAddress(       "maxFallPos",  maxFallPos);
    newtree->SetBranchAddress(         "slopeNeg",  slopeNeg);
    newtree->SetBranchAddress(        "intercNeg",  intercNeg);
    newtree->SetBranchAddress(         "chisqNeg",  chisqNeg);
    newtree->SetBranchAddress(        "correlNeg",  correlNeg);
    newtree->SetBranchAddress(        "maxAmpNeg",  maxAmpNeg);
    newtree->SetBranchAddress(       "maxRiseNeg",  maxRiseNeg);
    newtree->SetBranchAddress(       "maxFallNeg",  maxFallNeg);
    newtree->SetBranchAddress(      "reg700Start",  reg700Start);
    newtree->SetBranchAddress(        "reg700End",  reg700End);
    newtree->SetBranchAddress(    "classificVers", &classificVers);
    newtree->SetBranchAddress( "classifDctrlTest", &classificDctrlTest);
  }

  return newtree;
}

void ResetDctrlTestTreeVariables(void)
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
// Created:      08 Feb 2019  Mario Sitta
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
    slopePos[i] = 0;
    intercPos[i] = 0;
    chisqPos[i] = 0;
    correlPos[i] = 0;
    maxAmpPos[i] = 0;
    maxRisePos[i] = 0;
    maxFallPos[i] = 0;
    slopeNeg[i] = 0;
    intercNeg[i] = 0;
    chisqNeg[i] = 0;
    correlNeg[i] = 0;
    maxAmpNeg[i] = 0;
    maxRiseNeg[i] = 0;
    maxFallNeg[i] = 0;
  }
  driverSet = 0;
  peak2peakP = 0;
  peak2peakN = 0;
  amplitudeP = 0;
  amplitudeN = 0;
  riseTimeP = 0;
  riseTimeN = 0;
  fallTimeP = 0;
  fallTimeN = 0;
  worsMaxAmpl = 0;
  worsSlope = 0;
  worsSlopeRat = 0;
  worsChiSq = 0;
  worsChiSqRat = 0;
  worsCorrel = 0;
  worsRiseTim = 0;
  worsFallTim = 0;
  classificVers = 0;
  classificDctrlTest = 0;
}

TTree* SetupHicActListTreeDT(TFile *rootfile)
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
    newtree = CreateHicActListTreeDT();
  else
    newtree = ReadHicActListTreeDT(rootfile);

  return newtree;
}

TTree* SetupDctrlTestTree(TString treename, TString treetitle, TFile *rootfile)
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
    newtree = CreateTreeDctrlTest(treename,treetitle);
  else
    newtree = ReadDctrlTestTree(treename, rootfile);

  return newtree;
}

TTree* SetupDctrlTestTreeResult(TString treename, TString treetitle, TFile *rootfile)
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
    newtree = CreateTreeDctrlTestResult(treename,treetitle);
  else
    newtree = ReadDctrlTestTreeResult(treename, rootfile);

  return newtree;
}


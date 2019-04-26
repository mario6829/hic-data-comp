#ifndef DCTRLTESTLIB_H
#define DCTRLTESTLIB_H

#include <Rtypes.h>
#include <Riostream.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TLatex.h>
#include <TLegend.h>
#include <TMath.h>
#include <TPaveStats.h>
#include <TPaveText.h>
#include <TProfile.h>
#include <TString.h>
#include <TStyle.h>
#include <TTree.h>

#include "DBHelpers.h"
#include "AlpideDB.h"
#include "AlpideDBEndPoints.h"
#include "THIC.h"
#include "TScanFactory.h"

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

//Bool_t redoFromStart;

void analyzeAllDCTRLTests(std::vector<ComponentDB::componentShort> componentList, AlpideDB *db, const THicType hicType);
void analyzeDCTRLTest(const int hicid, const ComponentDB::compActivity act, AlpideDB *db, const THicType hicType);
void CopyDctrlTestOldToNew(const UInt_t hicid, const UInt_t actid, TTree *newscan, TTree *newres, TTree *oldscan, TTree *oldres);
TTree* CreateHicActListTreeDT(void);
TTree* CreateTreeDctrlTest(TString treeName, TString treeTitle);
TTree* CreateTreeDctrlTestResult(TString treeName, TString treeTitle);
void DctrlTestAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath);
void DctrlTestResults(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType);
Bool_t FillDctrlTestTree(TTree* tree, string path, string file);
Bool_t FillDctrlTestTreeResult(TTree* tree, string path, string file, ActivityDB::activityLong actlong, const THicType hicType);
Bool_t FindActivityInDctrlTestTree(TTree* listree, const UInt_t hicid, const UInt_t actid, const UShort_t mask);
TTree* ReadHicActListTreeDT(TFile *rootfile);
TTree* ReadDctrlTestTree(TString treename, TFile *rootfile);
TTree* ReadDctrlTestTreeResult(TString treename, TFile *rootfile);
void ResetDctrlTestTreeVariables(void);
TTree* SetupHicActListTreeDT(TFile *rootfile);
TTree* SetupDctrlTestTree(TString treename, TString treetitle, TFile *rootfile);
TTree* SetupDctrlTestTreeResult(TString treename, TString treetitle, TFile *rootfile);


#endif // DCTRLTESTLIB_H

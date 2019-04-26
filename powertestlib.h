#ifndef POWERTESTLIB_H
#define POWERTESTLIB_H

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

void analyzeAllPowerTests(std::vector<ComponentDB::componentShort> componentList, AlpideDB *db, const THicType hicType);
void analyzePowerTest(const int hicid, const ComponentDB::compActivity act, AlpideDB *db, const THicType hicType);
void CopyPowTestOldToNew(const UInt_t hicid, const UInt_t actid, TTree *newscan, TTree *newres, TTree *oldscan, TTree *oldres);
TTree* CreateHicActListTreePT(void);
TTree* CreateTreePowerTest(TString treeName, TString treeTitle);
TTree* CreateTreePowerTestResult(TString treeName, TString treeTitle);
void PowerTestAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath);
void PowerTestResults(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType);
Bool_t FillPowTestTree(TTree* tree, string path, string file);
Bool_t FillPowTestTreeResult(TTree* tree, string path, string file, ActivityDB::activityLong actlong, const THicType hicType);
Bool_t FindActivityInPowTestTree(TTree* listree, const UInt_t hicid, const UInt_t actid, const UShort_t mask);
TTree* ReadHicActListTreePT(TFile *rootfile);
TTree* ReadPowTestTree(TString treename, TFile *rootfile);
TTree* ReadPowTestTreeResult(TString treename, TFile *rootfile);
void ResetPowTestTreeVariables(void);
TTree* SetupHicActListTreePT(TFile *rootfile);
TTree* SetupPowTestTree(TString treename, TString treetitle, TFile *rootfile);
TTree* SetupPowTestTreeResult(TString treename, TString treetitle, TFile *rootfile);


#endif // POWERTESTLIB_H

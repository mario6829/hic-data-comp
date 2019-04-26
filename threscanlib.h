#ifndef THRESCANLIB_H
#define THRESCANLIB_H

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

void analyzeAllThresholdScans(std::vector<ComponentDB::componentShort> componentList, AlpideDB *db, const THicType hicType);
void analyzeThresholdScan(const int hicid, const ComponentDB::compActivity act, AlpideDB *db, const THicType hicType);
void CopyThreScanOldToNew(const UInt_t hicid, const UInt_t actid, TTree *newscan, TTree *newtun, TTree *newres, TTree *oldscan, TTree *oldtun, TTree *oldres);
TTree* CreateHicActListTreeTS(void);
TTree* CreateTreeThresholdScan(TString treeName, TString treeTitle);
TTree* CreateTreeThresholdScanResult(TString treeName, TString treeTitle);
Bool_t FillThreScanTree(TTree* tree, string path, string file);
Bool_t FillThreScanTreeResult(TTree* tree, string path, string file, ActivityDB::activityLong actlong, const THicType hicType);
Bool_t FindActivityInThreScanTree(TTree* listree, const UInt_t hicid, const UInt_t actid, const UShort_t mask);
TTree* ReadHicActListTreeTS(TFile *rootfile);
TTree* ReadThreScanTree(TString treename, TFile *rootfile);
TTree* ReadThreScanTreeResult(TString treename, TFile *rootfile);
void ResetThreScanTreeVariables(void);
void SanitizeThresScanInput(char *line);
TTree* SetupHicActListTreeTS(TFile *rootfile);
TTree* SetupThreScanTree(TString treename, TString treetitle, TFile *rootfile);
TTree* SetupThreScanTreeResult(TString treename, TString treetitle, TFile *rootfile);
void ThresholdScanAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType, bool allScans=true);
void ThresholdTuneAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType);
void ThresholdScanResults(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType);


#endif // THRESCANLIB_H

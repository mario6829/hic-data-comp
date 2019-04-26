#ifndef DIGISCANLIB_H
#define DIGISCANLIB_H

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

void analyzeAllDigitalScans(std::vector<ComponentDB::componentShort> componentList, AlpideDB *db, const THicType hicType);
void analyzeDigitalScan(const int hicid, const ComponentDB::compActivity act, AlpideDB *db, const THicType hicType);
void CopyDigScanOldToNew(const UInt_t hicid, const UInt_t actid, TTree *newscan, TTree *newres, TTree *oldscan, TTree *oldres);
TTree* CreateHicActListTreeDS(void);
TTree* CreateTreeDigitalScan(TString treeName, TString treeTitle);
TTree* CreateTreeDigitalScanResult(TString treeName, TString treeTitle);
void DigitalScanAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType);
void DigitalScanResults(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType);
Bool_t FillDigScanTree(TTree* tree, string path, string file);
Bool_t FillDigScanTreeResult(TTree* tree, string path, string file, ActivityDB::activityLong actlong, const THicType hicType);
Bool_t FindActivityInDigScanTree(TTree* listree, const UInt_t hicid, const UInt_t actid, const UShort_t mask);
TTree* ReadHicActListTreeDS(TFile *rootfile);
TTree* ReadDigScanTree(TString treename, TFile *rootfile);
TTree* ReadDigScanTreeResult(TString treename, TFile *rootfile);
void ResetDigScanTreeVariables(void);
TTree* SetupHicActListTreeDS(TFile *rootfile);
TTree* SetupDigScanTree(TString treename, TString treetitle, TFile *rootfile);
TTree* SetupDigScanTreeResult(TString treename, TString treetitle, TFile *rootfile);


#endif // DIGISCANLIB_H

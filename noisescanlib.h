#ifndef NOISESCANLIB_H
#define NOISESCANLIB_H

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

void analyzeAllNoiseScans(std::vector<ComponentDB::componentShort> componentList, AlpideDB *db, const THicType hicType);
void analyzeNoiseScan(const int hicid, const ComponentDB::compActivity act, AlpideDB *db, const THicType hicType);
void CopyNoiseScanOldToNew(const UInt_t hicid, const UInt_t actid, TTree *newscan, TTree *newres, TTree *oldscan, TTree *oldres);
TTree* CreateHicActListTreeNS(void);
TTree* CreateTreeNoiseScan(TString treeName, TString treeTitle);
TTree* CreateTreeNoiseScanResult(TString treeName, TString treeTitle);
Bool_t FillNoiseScanTree(TTree* tree, string path, string filepix, string filehits);
Bool_t FillNoiseScanTreeResult(TTree* tree, string path, string file, ActivityDB::activityLong actlong, const THicType hicType);
Bool_t FindActivityInNoiseScanTree(TTree* listree, const UInt_t hicid, const UInt_t actid, const UShort_t mask);
void NoiseScanAllChips(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath);
void NoiseScanResults(TTree *ftree, ActivityDB::activityLong actlong, const int hicid, const int actid, const string eospath, const THicType hicType);
TTree* ReadHicActListTreeNS(TFile *rootfile);
TTree* ReadNoiseScanTree(TString treename, TFile *rootfile);
TTree* ReadNoiseScanTreeResult(TString treename, TFile *rootfile);
void ResetNoiseScanTreeVariables(void);
TTree* SetupHicActListTreeNS(TFile *rootfile);
TTree* SetupNoiseScanTree(TString treename, TString treetitle, TFile *rootfile);
TTree* SetupNoiseScanTreeResult(TString treename, TString treetitle, TFile *rootfile);


#endif // NOISESCANLIB_H

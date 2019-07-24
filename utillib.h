#ifndef UTILLIB_H
#define UTILLIB_H

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
#include <TSystem.h>
#include <TTree.h>

#include "DBHelpers.h"
#include "AlpideDB.h"
#include "AlpideDBEndPoints.h"
#include "THIC.h"
#include "TScanFactory.h"
#include "TScanAnalysis.h"

#include <iostream>
#include <stdio.h>
#include <sys/stat.h>

#define ACTMASK_QUALIF 1
#define ACTMASK_RECEPT 2
#define ACTMASK_HALFST 4
#define ACTMASK_STAVET 8
#define ACTMASK_STVREC 16


Int_t AskUserRedoScan(void);
Bool_t CheckRootFileExists(TString name);
Int_t ChipPositionMAM2Test(const THicType hicType, const string position);
string ChipPositionTest2MAM(const THicType hicType, const Int_t position);
void CloseRootFile(TFile *rootfile);
Char_t ConvertTestResult(const string result);
string FindEOSPath(ActivityDB::activityLong actlong, const THicType hicType);
void FixActName(ActivityDB::activityLong &actlong, const THicType hicType);
TFile* OpenRootFile(TString name, Bool_t recreate=kFALSE);
Bool_t RenameExistingRootFile(TString oldname, TString mod, TString &newname);
TFile* SetupRootFile(TString name, Bool_t &redo);
void WaferNumAndPos(const THicType hicType, std::vector<TChild> children, const Int_t chipNum, Char_t &waferNum, Char_t &waferPos);


#endif // UTILLIB_H

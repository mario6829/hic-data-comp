#ifndef TREEVARIABLES_H
#define TREEVARIABLES_H

#define NUMCHIPS 14
#define NUMCHIPSIB 9

static Bool_t redoFromStart;

// Global tree variables
static UChar_t  condVB; // Conditions of the test: Voltage percentage + Bias (0,3)
static UChar_t  chipNum;
static UChar_t  staveOLML;
static UShort_t actMask;
static UInt_t   hicID;
static UInt_t   actID;
static ULong64_t startDate;
static Int_t    locID;
static Char_t   hicPosition; // HIC position in HS/Stave
static Float_t  vdddStart;
static Float_t  vdddEnd;
static Float_t  vddaStart;
static Float_t  vddaEnd;
static Float_t  vdddSetStart;
static Float_t  vdddSetEnd;
static Float_t  vddaSetStart;
static Float_t  vddaSetEnd;
static Float_t  idddStart;
static Float_t  idddEnd;
static Float_t  iddaStart;
static Float_t  iddaEnd;
static Float_t  anaSupVoltStart;
static Float_t  anaSupVoltEnd;
static Float_t  digSupVoltStart;
static Float_t  digSupVoltEnd;
static Float_t  tempStart;
static Float_t  tempEnd;
static Float_t  chipAnalVoltStart[NUMCHIPS];
static Float_t  chipAnalVoltEnd[NUMCHIPS];
static Float_t  chipDigiVoltStart[NUMCHIPS];
static Float_t  chipDigiVoltEnd[NUMCHIPS];
static Float_t  chipTempStart[NUMCHIPS];
static Float_t  chipTempEnd[NUMCHIPS];
static UShort_t reg700Start[NUMCHIPS];
static UShort_t reg700End[NUMCHIPS];
static Float_t  classificVers;
static Long64_t testOffset;
static Long64_t testResOffset;

#endif // TREEVARIABLES_H

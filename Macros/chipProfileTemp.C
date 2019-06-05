// Macro to plot histograms for Chip Temperature data
// Every plot is saved as a gif image
//
// Usage:
//    root [0] .x chipProfileTemp.C("test",condition,"startend","rootfile")
// where (* = default values if none entered)
//    test:  Q for Qualification test (*), R for Reception test,
//           H for Half-Stave test, S for Stave Qualification test
//           T for Stave Reception test
//    condition: 100 for nominal voltage no BB (*), 90 for 90% nominal voltage
//               110 for 110% nominal voltage, 103 for nominal voltage with BB
//    startend: Start to get Start values, End to get End values (*)
//    rootfile: root file produced by dataComp program
//              (* = OBHIC_DigitalScan_AllHICs.root)
//
//  06 Mar 2019    Mario Sitta (Mario.Sitta@cern.ch)
//
#define NUMCHIPS 14
#define MINTEMP 10.0
#define MAXTEMP 50.0

const Int_t maxCanvas = 30;
Int_t iCanvas = -1;
TCanvas *cc[maxCanvas];


void MakeTitles(TString &htitle,
                TString &gifname,
                const Int_t condvb)
{
  Int_t supply = (condvb/10)*10; // We deliberately divide int's
  Int_t vBB = condvb - supply;

  if (supply == 100) {
    htitle += "Nominal supply ";
    gifname += "_nomin";
  } else if (supply == 110) {
    htitle += "+10% supply ";
    gifname += "_upp";
  } else {
    htitle += "-10% supply ";
    gifname += "_low";
  }

  if (vBB == 0) {
    htitle += "No BB";
    gifname += "_nobb";
  } else {
    htitle += "BB 3V";
    gifname += "_bb";
  }
}


void PlotHisto(TH1F* histo,
               const TString gifName,
               const Bool_t logy=kFALSE,
               const Bool_t ovflow=kFALSE)
{
  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
  histo->Draw();

  if(logy) cc[iCanvas]->SetLogy();
  if(ovflow) {
    cc[iCanvas]->Update();
    TPaveStats *ps = (TPaveStats*)cc[iCanvas]->GetPrimitive("stats");
    ps->SetOptStat(101111);
    cc[iCanvas]->Update();
    cc[iCanvas]->Modified();
  }

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));

  return;
}


void PlotHisto(TH2F* histo,
               const TString gifName,
               const Bool_t colz=kTRUE,
               const Bool_t removeXstat=kTRUE)
{
  const Float_t resizFact = 1.10;

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
  if(colz)
    histo->Draw("COLZ");
  else
    histo->Draw();

  if(removeXstat) {
    // Freely copied from statsEditing.C
    cc[iCanvas]->Update();
    TPaveStats *ps = (TPaveStats*)cc[iCanvas]->GetPrimitive("stats");
    ps->SetName("mystats");
    TList *listOfLines = ps->GetListOfLines();
    TText *tmean = ps->GetLineWith("Mean ");
    listOfLines->Remove(tmean);
    TText *trms = ps->GetLineWith("RMS ");
    listOfLines->Remove(trms);
    ps->SetY1(resizFact*ps->GetY1()); // Move slightly up the lower margin
    histo->SetStats(0);
    cc[iCanvas]->Modified();
  //
  }

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));

  return;
}


void PlotHistoSuper(TH1F* histo1,
                    const char* name1,
                    TH1F* histo2,
                    const char* name2,
                    const TString gifName)
{
  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();

//  histo1->SetMaximum(380);
  histo1->SetLineColor(kBlack);
  histo1->SetStats(0);
  histo2->SetLineColor(kRed);
  histo2->SetStats(0);
  if(histo1->GetMaximum() >= histo2->GetMaximum()) {
    histo1->Draw();
    histo2->Draw("SAME");
  } else {
    histo2->Draw();
    histo1->Draw("SAME");
  }

  TLegend *theLegend = new TLegend(0.70, 0.60, 0.85, 0.75);
  theLegend->AddEntry(histo1, name1);
  theLegend->AddEntry(histo2, name2);
  theLegend->Draw();

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));

  return;
}


void PlotHistoChips(TH1F* histo,
                    const TString gifName,
                    const Float_t ytrans=0.02)
{
  TText txtlab;
  Float_t xlab, ylab;

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();

  histo->GetXaxis()->SetNdivisions(NUMCHIPS);
  histo->GetXaxis()->CenterLabels();
  histo->GetXaxis()->SetLabelOffset(99);
  histo->GetXaxis()->SetTitleOffset(1.3);
  histo->GetYaxis()->SetTitleOffset(1.3);
  histo->Draw();

  // This part is from a R.Brun example macro
//  ylab = gPad->GetUymin() - (gPad->GetUymax()-gPad->GetUymin())/25.;
  ylab = histo->GetMinimum() - ytrans;
  txtlab.SetTextSize(0.033);
  txtlab.SetTextAlign(22);
  txtlab.SetTextFont(42);
  for (Int_t i=0; i < NUMCHIPS; i++) {
    xlab = histo->GetXaxis()->GetBinCenter(i+1);
    Int_t chipn = i;
    if (chipn > 6) chipn++;
    txtlab.DrawText(xlab, ylab, Form("%d",chipn));
  } 

  // Freely copied from statsEditing.C
  cc[iCanvas]->Update();
  TPaveStats *ps = (TPaveStats*)cc[iCanvas]->GetPrimitive("stats");
  ps->SetName("mystats");
  TList *listOfLines = ps->GetListOfLines();
  TText *tmean = ps->GetLineWith("Mean ");
  listOfLines->Remove(tmean);
  TText *trms = ps->GetLineWith("RMS ");
  listOfLines->Remove(trms);
  ps->SetY1(ps->GetY1()+0.001);
  histo->SetStats(0);
  cc[iCanvas]->Modified();

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));

  return;
}


void chipProfileTemp(const char *test="Q",
                     const int   cond=100,
                     const char* startend="End",
                     const char *filename="OBHIC_DigitalScan_AllHICs.root")
{
  TFile *rootfile = new TFile(filename);
  if(!rootfile) {
    cout << "Error opening input file " << filename << endl;
    return;
  }

  TTree *qualTree = (TTree*)rootfile->Get("hicQualResTree");
  if(!qualTree) {
    cout << "Error getting hicQualTree tree" << endl;
    return;
  }

  TTree *recpTree = (TTree*)rootfile->Get("hicRecpResTree");
  if(!recpTree) {
    cout << "Error getting hicRecpTree tree" << endl;
    return;
  }

  TTree *hsTree = (TTree*)rootfile->Get("hicHSResTree");
  if(!hsTree) {
    cout << "Error getting hicHsTree tree" << endl;
    return;
  }

  TTree *staveQualTree = (TTree*)rootfile->Get("hicStaveQualResTree");
  if(!staveQualTree) {
    cout << "Error getting hicStaveQualTree tree" << endl;
    return;
  }

  TTree *staveRecpTree = (TTree*)rootfile->Get("hicStaveRecpResTree");
  if(!staveRecpTree) {
    cout << "Error getting hicStaveRecpTree tree" << endl;
    return;
  }

  TTree *currTree;
  char tCut[50], testName[10];

  char chosenTest = test[0];
  switch(chosenTest) {
    case 'Q':
      currTree = qualTree;
      strcpy(testName,"Qualif");
      break;
    case 'R':
      currTree = recpTree;
      strcpy(testName,"Recep");
      break;
    case 'H':
      currTree = hsTree;
      strcpy(testName,"HStave");
      break;
    case 'S':
      currTree = staveQualTree;
      strcpy(testName,"StaveQualif");
      break;
    case 'T':
      currTree = staveRecpTree;
      strcpy(testName,"StaveRecep");
      break;
    default:
      cout << "Unrecognized test name " << chosenTest << endl;
      cout << "Please use: Q for Qualification test, R for Reception test" << endl;
      cout << "            H for Half-Stave test, S for Stave Qualification test" << endl;
      cout << "            T for Stave Reception test" << endl;
      rootfile->Close();
      return;
      break; // Useless, I know... but keeps compiler quiet
  }

  // Book the profile histograms
  TH2F *starttemp = new TH2F("starttemp", "", NUMCHIPS, 0, NUMCHIPS, 100, 0, MAXTEMP);
  starttemp->GetXaxis()->SetTitle("Chip Number");
  starttemp->GetYaxis()->SetTitle("Temp (#circC)");

  TH2F *endtemp = new TH2F("endtemp", "", NUMCHIPS, 0, NUMCHIPS, 100, 0, MAXTEMP);
  endtemp->GetXaxis()->SetTitle("Chip Number");
  endtemp->GetYaxis()->SetTitle("Temp (#circC)");

  TH2F *difftemp = new TH2F("difftemp", "", NUMCHIPS, 0, NUMCHIPS, 100, -10., 10.);
  difftemp->GetXaxis()->SetTitle("Chip Number");
  difftemp->GetYaxis()->SetTitle("Temp (#circC)");

  TProfile *tempa = new TProfile("tempa", "", NUMCHIPS, 0, NUMCHIPS);
  tempa->GetXaxis()->SetTitle("Chip Number");
  tempa->GetYaxis()->SetTitle("Temp (#circC)");

  TProfile *tempc = new TProfile("tempc", "", NUMCHIPS, 0, NUMCHIPS);
  tempc->GetXaxis()->SetTitle("Chip Number");
  tempc->GetYaxis()->SetTitle("Temp (#circC)");

  UChar_t  condVB;
  Float_t  chipTempStart[NUMCHIPS];
  Float_t  chipTempEnd[NUMCHIPS];

  currTree->SetBranchAddress("condVB"       , &condVB      );
  currTree->SetBranchAddress("chipTempStart", chipTempStart);
  currTree->SetBranchAddress("chipTempEnd"  , chipTempEnd  );

  // First loop
  Int_t nEntries = currTree->GetEntries();

  for(Int_t jent = 0; jent < nEntries; jent++) {
    currTree->GetEntry(jent);

    if (condVB == cond) {
      for (Int_t jchip = 0; jchip < NUMCHIPS; jchip++) {
        if(chipTempStart[jchip] > MINTEMP)
          starttemp->Fill(jchip, chipTempStart[jchip]);
        if(chipTempEnd[jchip] > MINTEMP)
          endtemp->Fill(jchip, chipTempEnd[jchip]);
        if(chipTempStart[jchip] > MINTEMP && chipTempStart[jchip] < MAXTEMP &&
           chipTempEnd[jchip] > MINTEMP && chipTempEnd[jchip] < MAXTEMP)
          difftemp->Fill(jchip, chipTempEnd[jchip] - chipTempStart[jchip]);
        if(strcmp(startend,"Start") == 0) {
          tempa->Fill(jchip, chipTempStart[jchip]);
          if(chipTempStart[jchip] > MINTEMP && chipTempStart[jchip] < MAXTEMP)
            tempc->Fill(jchip, chipTempStart[jchip]);
        } else {
          tempa->Fill(jchip, chipTempEnd[jchip]);
          if(chipTempEnd[jchip] > MINTEMP && chipTempEnd[jchip] < MAXTEMP)
            tempc->Fill(jchip, chipTempEnd[jchip]);
        }
      }
    }
  }

  // Book the dispersion histograms
  Float_t rmsY = tempa->GetRMS(2); // Mean Y RMS
  TH2F *tempdispa = new TH2F("tempdispa", "", NUMCHIPS, 0, NUMCHIPS, 100, -rmsY/4, rmsY/4);
  tempdispa->GetXaxis()->SetTitle("Chip number");
  tempdispa->GetYaxis()->SetTitle("Temperature Diff (#circC)");

  rmsY = tempc->GetRMS(2); // Mean Y RMS
  TH2F *tempdispc = new TH2F("tempdispc", "", NUMCHIPS, 0, NUMCHIPS, 100, -3*rmsY, 3*rmsY);
  tempdispc->GetXaxis()->SetTitle("Chip number");
  tempdispc->GetYaxis()->SetTitle("Temperature Diff (#circC)");

  // Second loop
  Float_t meanTempA, meanTempC;
  for(Int_t jent = 0; jent < nEntries; jent++) {
    currTree->GetEntry(jent);

    if (condVB == cond) {
      for (Int_t jchip = 0; jchip < NUMCHIPS; jchip++) {
        meanTempA = tempa->GetBinContent(jchip+1);
        meanTempC = tempc->GetBinContent(jchip+1);
        if(strcmp(startend,"Start") == 0) {
          tempdispa->Fill(jchip, chipTempStart[jchip] - meanTempA);
          if(chipTempStart[jchip] > MINTEMP && chipTempStart[jchip] < MAXTEMP)
            tempdispc->Fill(jchip, chipTempStart[jchip] - meanTempC);
        } else {
          tempdispa->Fill(jchip, chipTempEnd[jchip] - meanTempA);
          if(chipTempEnd[jchip] > MINTEMP && chipTempEnd[jchip] < MAXTEMP)
            tempdispc->Fill(jchip, chipTempEnd[jchip] - meanTempC);
        }
      }
    }
  }

  // Plot all histos
  TString newTitle, gifName;
  TH1F *hproj;

  newTitle  = Form("Start temperature (> %.1fC) (all chips) - %s Test - ", MINTEMP, testName); 
  gifName = Form("startempall%s", testName);
  MakeTitles(newTitle, gifName, cond);

  starttemp->SetTitle(newTitle.Data());
  starttemp->GetYaxis()->SetTitleOffset(1.4);
  PlotHisto((TH1F*)starttemp->ProjectionY(), gifName, kFALSE, kTRUE);

  newTitle  = Form("End temperature (> %.1fC) (all chips) - %s Test - ", MINTEMP, testName); 
  gifName = Form("endtempall%s", testName);
  MakeTitles(newTitle, gifName, cond);

  endtemp->SetTitle(newTitle.Data());
  endtemp->GetYaxis()->SetTitleOffset(1.4);
  PlotHisto((TH1F*)endtemp->ProjectionY(), gifName, kFALSE, kTRUE);

  newTitle  = Form("Start and End temperatures (> %.1fC) (all chips) - %s Test - ", (Float_t)MINTEMP, testName); 
  gifName = Form("startendsuper%s", testName);
  MakeTitles(newTitle, gifName, cond);

  starttemp->ProjectionY()->SetTitle(newTitle.Data());
  PlotHistoSuper((TH1F*)starttemp->ProjectionY(), "Start T",
                 (TH1F*)endtemp->ProjectionY(), "End T", gifName);

  newTitle  = Form("End-Start temperature difference (all chips) - %s Test - ", testName); 
  gifName = Form("endstartdiff2D%s", testName);
  MakeTitles(newTitle, gifName, cond);

  difftemp->SetTitle(newTitle.Data());
  difftemp->GetXaxis()->SetNdivisions(NUMCHIPS);
  difftemp->GetXaxis()->CenterLabels();
  PlotHisto(difftemp, gifName, kFALSE);

  gifName = Form("endstartdiff%s", testName);
  MakeTitles(newTitle, gifName, cond);

  PlotHisto((TH1F*)difftemp->ProjectionY(), gifName);

  newTitle = Form("Mean %s temperature per chip (all values) - %s Test - ",
                  startend, testName);
  gifName = Form("tempa%s", testName);
  MakeTitles(newTitle, gifName, cond);

  tempa->SetTitle(newTitle.Data());
  PlotHistoChips((TH1F*)tempa, gifName, 20.);

  newTitle = Form("Mean %s temperature (%.1f - %.1f C) per chip - %s Test - ",
                  startend, MINTEMP, MAXTEMP, testName);
  gifName = Form("tempc%s%s", startend, testName);
  MakeTitles(newTitle, gifName, cond);

  tempc->SetTitle(newTitle.Data());
  PlotHistoChips((TH1F*)tempc, gifName, 0.345);

  newTitle = Form("Dispersion of %s temperature (all values) - %s Test - ",
                  startend, testName);
  gifName = Form("disptempa2D%s%s", startend, testName);
  MakeTitles(newTitle, gifName, cond);

  tempdispa->SetTitle(newTitle.Data());
  tempdispa->GetXaxis()->SetNdivisions(NUMCHIPS);
  tempdispa->GetXaxis()->CenterLabels();
  PlotHisto(tempdispa, gifName, kFALSE);

  gifName = Form("disptempa%s%s", startend, testName);
  MakeTitles(newTitle, gifName, cond);
  PlotHisto((TH1F*)tempdispa->ProjectionY(), gifName, kFALSE, kTRUE);

  newTitle = Form("Dispersion of %s temperature (%.1f - %.1f C) - %s Test - ",
                  startend, MINTEMP, MAXTEMP, testName);
  gifName = Form("disptempa2D%s%s", startend, testName);
  MakeTitles(newTitle, gifName, cond);

  tempdispc->SetTitle(newTitle.Data());
  tempdispc->GetXaxis()->SetNdivisions(NUMCHIPS);
  tempdispc->GetXaxis()->CenterLabels();
  PlotHisto(tempdispc, gifName, kFALSE);

  gifName = Form("disptempc%s%s", startend, testName);
  MakeTitles(newTitle, gifName, cond);
  PlotHisto((TH1F*)tempdispc->ProjectionY(), gifName, kFALSE, kTRUE);

  //
  return;
}


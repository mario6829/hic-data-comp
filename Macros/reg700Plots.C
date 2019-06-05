// Macro to plot histograms for Register 0x700 data
// Every plot is saved as a gif image
//
// Usage:
//    root [0] .x reg700Plots.C("test",condition,"rootfile")
// where (* = default values if none entered)
//    test:  Q for Qualification test (*), R for Reception test,
//           H for Half-Stave test, S for Stave Qualification test
//           T for Stave Reception test
//    condition: 100 for nominal voltage no BB (*), 90 for 90% nominal voltage
//               110 for 110% nominal voltage, 103 for nominal voltage with BB
//    rootfile: root file produced by dataComp program
//              (* = OBHIC_DigitalScan_AllHICs.root)
//
//  06 May 2019    Mario Sitta (Mario.Sitta@cern.ch)
//
#define NUMCHIPS 14

const Int_t numSites = 5;

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
               const Bool_t removeXstat=kTRUE,
               const Float_t resizFact=1.10)
{
  TH1F *hprof;

  TText txtlab;
  Float_t xlab, ylab;

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
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
}


void PlotHisto(TH2F* histo,
               const TString gifName,
               const Bool_t removeXstat=kTRUE,
               const Float_t resizFact=1.10)
{
  TH1F *hprof;

  TText txtlab;
  Float_t xlab, ylab;

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
  histo->Draw("COLZ");

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
}


void PlotHistoChips(TH1F* histo,
                    const TString gifName,
                    const Float_t ytrans=0.02,
                    const Bool_t removeXstat=kTRUE)
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
    ps->SetY1(ps->GetY1()+0.001);
    histo->SetStats(0);
    cc[iCanvas]->Modified();
  }

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));

  return;
}


void PlotHistoChips(TH2F* histo,
                    const TString gifName,
                    const Float_t ylab=1)
{
  TText txtlab;
  Float_t xlab;

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();

  histo->GetXaxis()->SetNdivisions(NUMCHIPS);
  histo->GetXaxis()->CenterLabels();
  histo->GetXaxis()->SetLabelOffset(99);
  histo->GetXaxis()->SetTitleOffset(1.3);
  histo->GetYaxis()->SetTitleOffset(1.3);
  histo->Draw("COLZ");

  // This part is from a R.Brun example macro
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


void Plot1DZoomX(TH1F* histo,
                 const TString gifName,
                 const Int_t maxXbin,
                 const Int_t minXbin=1,
                 const Bool_t logy=kFALSE)
{
  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
  histo->SetStats(0);
  histo->GetXaxis()->SetRange(minXbin, maxXbin);
  histo->Draw();

  if(logy) cc[iCanvas]->SetLogy();

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));
}


void Plot2DZoomXY(TH2F* histo,
                  const TString gifName,
                  const Int_t maxXbin,
                  const Int_t maxYbin,
                  const Int_t minXbin=1,
                  const Int_t minYbin=1,
                  const Float_t ylab = -1.)
{
  TText txtlab;
  Float_t xlab;

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
  histo->SetStats(0);
  histo->GetXaxis()->SetRange(minXbin, maxXbin);
  histo->GetYaxis()->SetRange(minYbin, maxYbin);
  histo->Draw("COLZ");

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));
}


void Plot2DZoomXYChips(TH2F* histo,
                       const TString gifName,
                       const Int_t maxXbin,
                       const Int_t maxYbin,
                       const Int_t minXbin=1,
                       const Int_t minYbin=1,
                       const Float_t ylab = -1.)
{
  TText txtlab;
  Float_t xlab;

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
  histo->SetStats(0);
  histo->GetXaxis()->SetRange(minXbin, maxXbin);
  histo->GetYaxis()->SetRange(minYbin, maxYbin);

  histo->GetXaxis()->SetNdivisions(NUMCHIPS);
  histo->GetXaxis()->CenterLabels();
  histo->GetXaxis()->SetLabelOffset(99);
  histo->GetXaxis()->SetTitleOffset(1.3);

  histo->Draw("COLZ");

  // This part is from a R.Brun example macro
  txtlab.SetTextSize(0.033);
  txtlab.SetTextAlign(22);
  txtlab.SetTextFont(42);
  for (Int_t i=0; i < NUMCHIPS; i++) {
    xlab = histo->GetXaxis()->GetBinCenter(i+1);
    Int_t chipn = i;
    if (chipn > 6) chipn++;
    txtlab.DrawText(xlab, ylab, Form("%d",chipn));
  } 

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));
}


void reg700Plots(const char *test="Q",
                 const int   cond=100,
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

  TH2F *start700 = new TH2F("start700","",NUMCHIPS,0,NUMCHIPS,100,-0.5,99.5);
  start700->GetXaxis()->SetTitle("Chip n.");
  start700->GetYaxis()->SetTitle("Reg 0x700");

  TH2F *end700 = new TH2F("end700","",NUMCHIPS,0,NUMCHIPS,100,-0.5,99.5);
  end700->GetXaxis()->SetTitle("Chip n.");
  end700->GetYaxis()->SetTitle("Reg 0x700");

  TH2F *diff700 = new TH2F("diff700","",NUMCHIPS,0,NUMCHIPS,101,-50.5,50.5);
  diff700->GetXaxis()->SetTitle("Chip n.");
  diff700->GetYaxis()->SetTitle("Reg 0x700");

  TH2F *endvsstart = new TH2F("endvsstart","",100,-0.5,99.5,100,-0.5,99.5);
  endvsstart->GetXaxis()->SetTitle("Reg 0x700 Start");
  endvsstart->GetYaxis()->SetTitle("Reg 0x700 End");

  TH1F *startnot0 = new TH1F("startnot0","",NUMCHIPS,0,NUMCHIPS);
  startnot0->GetXaxis()->SetTitle("Chip n.");

  TH1F *startgtend = new TH1F("startgtend","",NUMCHIPS,0,NUMCHIPS);
  startgtend->GetXaxis()->SetTitle("Chip n.");

  TH2F *start700gtend = new TH2F("start700gtend","",NUMCHIPS,0,NUMCHIPS,100,-0.5,99.5);
  start700gtend->GetXaxis()->SetTitle("Chip n.");
  start700gtend->GetYaxis()->SetTitle("Reg 0x700");

  TH1F *nstnot0 = new TH1F("nstnot0","",NUMCHIPS+1,0,NUMCHIPS+1);

  TH1F *nstgtend = new TH1F("nstgtend","",NUMCHIPS+1,0,NUMCHIPS+1);

  UChar_t  condVB;
  UShort_t reg700Start[NUMCHIPS];
  UShort_t reg700End[NUMCHIPS];

  currTree->SetBranchAddress("condVB"     , &condVB    );
  currTree->SetBranchAddress("reg700Start", reg700Start);
  currTree->SetBranchAddress("reg700End"  , reg700End  );

  Int_t nEntries = currTree->GetEntries();
  Int_t nStartNot0, nStartGtEnd;
  for(Int_t jent = 0; jent < nEntries; jent++) {
    currTree->GetEntry(jent);

    if (condVB == cond) {
      nStartNot0 = 0;
      nStartGtEnd = 0;
      for (Int_t jchip = 0; jchip < 14; jchip++) {
        start700->Fill(jchip, reg700Start[jchip]);
        end700->Fill(jchip, reg700End[jchip]);
        diff700->Fill(jchip, (reg700End[jchip] - reg700Start[jchip]));
        endvsstart->Fill(reg700Start[jchip], reg700End[jchip]);

        if(reg700Start[jchip] > 0) {
	  startnot0->Fill(jchip);
	  nStartNot0++;
	}
        if(reg700Start[jchip] > reg700End[jchip]) {
          startgtend->Fill(jchip);
          start700gtend->Fill(jchip, reg700Start[jchip]);
	  nStartGtEnd++;
        }
      }
      nstnot0->Fill(nStartNot0);
      nstgtend->Fill(nStartGtEnd);
    }
  }

  TString newTitle, newGifName;
  TH1F *htemp;

  newTitle = Form("Reg 0x700 Start - %s Test - ", testName);
  newGifName = Form("start700%s_2D",testName);
  MakeTitles(newTitle, newGifName, cond);
  newTitle += " - All chips";
  start700->SetTitle(newTitle.Data());
  start700->GetXaxis()->SetNdivisions(NUMCHIPS);
  start700->GetXaxis()->CenterLabels();
  PlotHisto(start700, newGifName);

  newGifName = Form("start700%s_allchips",testName);
  MakeTitles(newTitle, newGifName, cond);
  PlotHisto((TH1F*)start700->ProjectionY(), newGifName, kFALSE);

  newGifName = Form("start700%s_perchip",testName);
  MakeTitles(newTitle, newGifName, cond);
  PlotHistoChips((TH1F*)start700->ProfileX(), newGifName, 0.06);

  newGifName = Form("start700%s_perchip_zoom",testName);
  MakeTitles(newTitle, newGifName, cond);
  Plot2DZoomXYChips(start700, newGifName, NUMCHIPS, 15);

  newGifName = Form("start700%s_allchips_zoom",testName);
  MakeTitles(newTitle, newGifName, cond);
  start700->ProjectionY()->GetXaxis()->CenterLabels();
  Plot1DZoomX((TH1F*)start700->ProjectionY(), newGifName, 15);

  newGifName = Form("start700%s_allchips_zoom_logy",testName);
  MakeTitles(newTitle, newGifName, cond);
  Plot1DZoomX((TH1F*)start700->ProjectionY(), newGifName, 15, 1, kTRUE);

  newTitle = Form("Reg 0x700 End - %s Test - ", testName);
  newGifName = Form("end700%s_2D",testName);
  MakeTitles(newTitle, newGifName, cond);
  newTitle += " - All chips";
  end700->SetTitle(newTitle.Data());
  end700->GetXaxis()->SetNdivisions(NUMCHIPS);
  end700->GetXaxis()->CenterLabels();
  PlotHisto(end700, newGifName);

  newGifName = Form("end700%s_allchips",testName);
  MakeTitles(newTitle, newGifName, cond);
  PlotHisto((TH1F*)end700->ProjectionY(), newGifName, kFALSE);

  newGifName = Form("end700%s_perchip",testName);
  MakeTitles(newTitle, newGifName, cond);
  PlotHistoChips((TH1F*)end700->ProfileX(), newGifName, 0.16);

  newGifName = Form("end700%s_perchip_zoom",testName);
  MakeTitles(newTitle, newGifName, cond);
  Plot2DZoomXYChips(end700, newGifName, NUMCHIPS, 10);

  newGifName = Form("end700%s_allchips_zoom",testName);
  MakeTitles(newTitle, newGifName, cond);
  end700->ProjectionY()->GetXaxis()->CenterLabels();
  Plot1DZoomX((TH1F*)end700->ProjectionY(), newGifName, 15);

  newGifName = Form("end700%s_allchips_zoom_logy",testName);
  MakeTitles(newTitle, newGifName, cond);
  Plot1DZoomX((TH1F*)end700->ProjectionY(), newGifName, 15, 1, kTRUE);

  newTitle = Form("Reg 0x700 Difference End-Start - %s Test - ", testName);
  newGifName = Form("diff700%s_2D",testName);
  MakeTitles(newTitle, newGifName, cond);
  newTitle += " - All chips";
  diff700->SetTitle(newTitle.Data());
  diff700->GetXaxis()->SetNdivisions(NUMCHIPS);
  diff700->GetXaxis()->CenterLabels();
  PlotHisto(diff700, newGifName, kTRUE, 1.30);

  newGifName = Form("diff700%s_allchips",testName);
  MakeTitles(newTitle, newGifName, cond);
  PlotHisto((TH1F*)diff700->ProjectionY(), newGifName, kFALSE);

  newGifName = Form("diff700%s_perchip",testName);
  MakeTitles(newTitle, newGifName, cond);
  PlotHistoChips((TH1F*)diff700->ProfileX(), newGifName, 0.12);

  newGifName = Form("diff700%s_perchip_zoom",testName);
  MakeTitles(newTitle, newGifName, cond);
  Plot2DZoomXYChips(diff700, newGifName, NUMCHIPS, 60, 1, 42, -10.);

  newGifName = Form("diff700%s_allchips_zoom",testName);
  MakeTitles(newTitle, newGifName, cond);
  diff700->ProjectionY()->GetXaxis()->CenterLabels();
  Plot1DZoomX((TH1F*)diff700->ProjectionY(), newGifName, 19);

  newGifName = Form("diff700%s_allchips_zoom_logy",testName);
  MakeTitles(newTitle, newGifName, cond);
  Plot1DZoomX((TH1F*)diff700->ProjectionY(), newGifName, 19, 1, kTRUE);

  newTitle  = Form("Reg 0x700 End Vs Start - %s Test - ", testName);
  newGifName = Form("endvsstart700%s",testName);
  MakeTitles(newTitle, newGifName, cond);

  endvsstart->SetTitle(newTitle.Data());
  PlotHisto(endvsstart, newGifName, kFALSE);

  newGifName = Form("endvsstart700%s_allchips_zoom",testName);
  MakeTitles(newTitle, newGifName, cond);
  Plot2DZoomXY(endvsstart, newGifName, 16, 16);

  newTitle = Form("Reg 0x700 Start > 0 - %s Test - ", testName);
  newGifName = Form("start700not0%s",testName);
  MakeTitles(newTitle, newGifName, cond);

  startnot0->SetTitle(newTitle.Data());
//  PlotHistoChips(startnot0, newGifName, kFALSE);

  newGifName = Form("start700not0%s_fullscale",testName);
  MakeTitles(newTitle, newGifName, cond);
  startnot0->SetMinimum(0.);
  PlotHistoChips(startnot0, newGifName, 24, kFALSE);

  newTitle  = Form("Reg 0x700 Start > End - %s Test - ", testName);
  newGifName = Form("start700gtEnd%s",testName);
  MakeTitles(newTitle, newGifName, cond);
  startgtend->SetTitle(newTitle.Data());
//  PlotHisto(startgtend, sgteGifName, kFALSE);

  newGifName = Form("start700gtEnd%s_fullscale",testName);
  MakeTitles(newTitle, newGifName, cond);
  startgtend->SetMinimum(0.);
  PlotHistoChips(startgtend, newGifName, 9);

  newTitle = Form("Reg 0x700 Start When > End - %s Test - ", testName);
  newGifName = Form("start700whenGtEnd%s",testName);
  MakeTitles(newTitle, newGifName, cond);

  start700gtend->SetTitle(newTitle.Data());
  PlotHistoChips(start700gtend, newGifName, -4);

  newGifName = Form("start700whenGtEnd%s_zoom",testName);
  Plot2DZoomXY(start700gtend, newGifName, NUMCHIPS, 10);

  newTitle = Form("Number of chips with Reg 0x700 Start > 0 - %s Test - ", testName);
  newGifName = Form("numstart700not0%s",testName);
  MakeTitles(newTitle, newGifName, cond);

  nstnot0->SetTitle(newTitle.Data());
  nstnot0->GetXaxis()->SetNdivisions(NUMCHIPS+1);
  nstnot0->GetXaxis()->CenterLabels();
  PlotHisto(nstnot0, newGifName, kFALSE);

  newTitle = Form("Number of chips with Reg 0x700 Start > End - %s Test - ", testName);
  newGifName = Form("numstart700GtEnd%s",testName);
  MakeTitles(newTitle, newGifName, cond);

  nstgtend->SetTitle(newTitle.Data());
  nstgtend->GetXaxis()->SetNdivisions(NUMCHIPS+1);
  nstgtend->GetXaxis()->CenterLabels();
  PlotHisto(nstgtend, newGifName, kFALSE);

  //
  return;
}


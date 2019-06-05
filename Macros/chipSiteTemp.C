// Macro to plot histograms some histograms on temperature
// and compare the different sites
// Every plot is saved as a gif image
//
// Usage:
//    root [0] .x chipSiteTemp.C("test",condition,"rootfile")
// where (* = default values if none entered)
//    test:  Q for Qualification test (*), R for Reception test,
//           H for Half-Stave test, S for Stave Qualification test
//           T for Stave Reception test
//    condition: 100 for nominal voltage no BB (*), 90 for 90% nominal voltage
//               110 for 110% nominal voltage, 103 for nominal voltage with BB
//    rootfile: root file produced by dataComp program
//              (* = OBHIC_DigitalScan_AllHICs.root)
//
//  20 Feb 2019    Mario Sitta (Mario.Sitta@cern.ch)
//
#define NUMCHIPS 14
#define MINTEMP 0
#define MAXTEMP 50

const Int_t numSites = 5;

const Int_t maxCanvas = 30;
Int_t iCanvas = -1;
TCanvas *cc[maxCanvas];


Int_t LocID2Site(const Int_t locID, const char *test="Q")
{
  Int_t locQual[numSites] = {441, 442, 445, 446, 461};
  Int_t locRecp[numSites] = {542, 543, 544, 545, 546};
  Int_t locHS[numSites] = {841, 782, 784, 785, 786};
  Int_t locStaveQual[numSites] = {1121, 1101, 1102, 1103, 1104};
  Int_t locStaveRecp[2] = {1181, 1182};

  Int_t site = 0;
  for(Int_t j=0; j<numSites; j++) {
    if(strcmp(test,"Q") == 0 && locQual[j] == locID) {
      site = j+1;
      break;
    }
    if(strcmp(test,"R") == 0 && locRecp[j] == locID) {
      site = j+1;
      break;
    }
    if(strcmp(test,"H") == 0 && locHS[j] == locID) {
      site = j+1;
      break;
    }
    if(strcmp(test,"S") == 0 && locStaveQual[j] == locID) {
      site = j+1;
      break;
    }
    if(strcmp(test,"T") == 0 && j < 2 && locStaveRecp[j] == locID) {
      site = j+1;
      break;
    }
  }

  return site;
}


TString Site2Name(const Int_t site, const char *test="Q")
{
  TString locNameQual[numSites] = {"Wuhan", "Pusan", "Strasbourg", "Liverpool", "Bari"};
  TString locNameRecp[numSites] = {"Frascati", "Berkely", "Nikhef", "Daresbury", "Torino"};
  TString locNameHS[numSites] = {"Berkely (ML)", "Frascati (OL)", "Nikhef (OL)", "Daresbury (OL)", "Torino (OL)"};
  TString locNameStaveQual[numSites] = {"Berkely (ML)", "Torino (OL)", "Daresbury (OL)", "Nikhef (OL)", "Frascati (OL)"};
  TString locNameStaveRecp[2] = {"CERN (OL)", "CERN (ML)"};

  char chosenTest = test[0];
  switch(chosenTest) {
    case 'Q':
      return locNameQual[site-1];
      break;
    case 'R':
      return locNameRecp[site-1];
      break;
    case 'H':
      return locNameHS[site-1];
      break;
    case 'S':
      return locNameStaveQual[site-1];
      break;
    case 'T':
      if(site < 3)
	return locNameStaveRecp[site-1];
      else
	return "";
      break;
    default:
      return "";
      break;
  }
}


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
               const TString gifName)
{
  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
  histo->Draw("COLZ");

  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));

  return;
}


void PlotHistoVsSite(TH2F* histo,
                     const TString gifName,
                     const char *test="Q",
                     const Bool_t removeXstat=kTRUE)
{
  TText txtlab;
  Float_t xlab, ylab;

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();
  histo->GetXaxis()->SetNdivisions(numSites);
  histo->GetXaxis()->CenterLabels();
  histo->GetXaxis()->SetLabelOffset(99);
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
    ps->SetY1(1.1*ps->GetY1()); // Move slightly up the lower margin
    histo->SetStats(0);
    cc[iCanvas]->Modified();
  //
  }

  // This part is from a R.Brun example macro
  ylab = gPad->GetUymin() - (gPad->GetUymax()-gPad->GetUymin())/25.;
  txtlab.SetTextSize(0.035);
  txtlab.SetTextAlign(22);
  txtlab.SetTextFont(42);
  for (Int_t i=0; i < histo->GetNbinsX(); i++) {
    xlab = histo->GetXaxis()->GetBinCenter(i+1);
    txtlab.DrawText(xlab, ylab, Site2Name(i+1,test).Data());
  } 
  //
  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));

  return;
}


void PlotAllSites(TH2F* histo,
                  const TString gifName,
                  const Int_t *siteOrder,
                  TLegend *siteLegend,
		  const char *test="Q")
{
  const Int_t siteColor[numSites] = {2, 4, 6, 8, 1};

  TH1F *siteHisto[numSites];

  iCanvas++;
  cc[iCanvas] = new TCanvas(Form("c%d",iCanvas),Form("c%d",iCanvas),700,500);
  cc[iCanvas]->cd();

  for (Int_t i = 0; i<numSites; i++) {
    Int_t j = siteOrder[i];
    siteHisto[i] = (TH1F*)histo->ProjectionY(Form("px%d",j),j,j)->Clone();
    siteHisto[i]->SetLineColor(siteColor[j-1]);
    siteHisto[i]->SetStats(kFALSE);
    if(i == 0) {
      siteHisto[i]->SetTitle(histo->GetTitle());
      siteHisto[i]->Draw();
    } else
      siteHisto[i]->Draw("SAME");
    siteLegend->AddEntry(siteHisto[i],Site2Name(j,test).Data());
  }
  siteLegend->Draw();
  cc[iCanvas]->Print(Form("%s.gif",gifName.Data()));

  return;
}


void chipSiteTemp(const char *test="Q",
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
  char testName[10];

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

  // Book the histograms
  TH1F *tstart = new TH1F("tstart","",100,0.,MAXTEMP);
  tstart->GetXaxis()->SetTitle("Temp (#circC)");

  TH1F *tend = new TH1F("tend","",100,0.,MAXTEMP);
  tend->GetXaxis()->SetTitle("Temp (#circC)");

  TH2F *starttemp = new TH2F("starttemp","",5,1,6,100,0.,MAXTEMP);
  starttemp->GetXaxis()->SetTitle("Site");
  starttemp->GetYaxis()->SetTitle("Temp (#circC)");

  TH2F *endtemp = new TH2F("endtemp","",5,1,6,100,0.,MAXTEMP);
  endtemp->GetXaxis()->SetTitle("Site");
  endtemp->GetYaxis()->SetTitle("Temp (#circC)");

  TH2F *difftemp = new TH2F("difftemp","",5,1,6,100,-20,20);
  difftemp->GetXaxis()->SetTitle("Site");
  difftemp->GetYaxis()->SetTitle("Temp (#circC)");

  TH2F *tempVsVddaStart[numSites], *tempVsVddaEnd[numSites];
  for (Int_t i = 0; i<numSites; i++) {
    tempVsVddaStart[i] = new TH2F(Form("tvdda%dS",i+1),Form("tvdda%dS",i+1),
                                  100,0,2.5,100,MINTEMP,MAXTEMP);
    tempVsVddaEnd[i] = new TH2F(Form("tvdda%dE",i+1),Form("tvdda%dE",i+1),
                                100,0,2.5,100,MINTEMP,MAXTEMP);
  }  

  UChar_t  condVB;
  Int_t    locID;
  Float_t  vddaStart;
  Float_t  vddaEnd;
  Float_t  tempStart;
  Float_t  tempEnd;
  Float_t  chipTempStart[NUMCHIPS];
  Float_t  chipTempEnd[NUMCHIPS];

  currTree->SetBranchAddress("locID"        , &locID       );
  currTree->SetBranchAddress("condVB"       , &condVB      );
  currTree->SetBranchAddress("vddaStart"    , &vddaStart   );
  currTree->SetBranchAddress("vddaEnd"      , &vddaEnd     );
  currTree->SetBranchAddress("tempStart"    , &tempStart   );
  currTree->SetBranchAddress("tempEnd"      , &tempEnd     );
  currTree->SetBranchAddress("chipTempStart", chipTempStart);
  currTree->SetBranchAddress("chipTempEnd"  , chipTempEnd  );

  // Loop on entries
  Int_t nEntries = currTree->GetEntries();
  for(Int_t jent = 0; jent < nEntries; jent++) {
    currTree->GetEntry(jent);
    if (condVB == cond) {
      if(tempStart > 0 && tempEnd > 0) {
        tstart->Fill(tempStart);
        tend->Fill(tempEnd);
      }
      Int_t site = LocID2Site(locID, test);
      if(site != 0) {
        starttemp->Fill(site, tempStart);
        endtemp->Fill(site, tempEnd);
        difftemp->Fill(site,(tempEnd - tempStart));
        tempVsVddaStart[site-1]->Fill(vddaStart, tempStart);
        tempVsVddaEnd[site-1]->Fill(vddaEnd, tempEnd);
      }
    }
  }

  // Plot all histos
  TString newTitle, gifName;

  newTitle = Form("Temperature T_{Start} - %s Test - ", testName);
  gifName = Form("startemp%s",testName);
  MakeTitles(newTitle, gifName, cond);

  tstart->SetTitle(newTitle.Data());
  PlotHisto(tstart, gifName, kFALSE, kTRUE);

  newTitle = Form("Temperature T_{End} - %s Test - ", testName);
  gifName = Form("endtemp%s",testName);
  MakeTitles(newTitle, gifName, cond);

  tend->SetTitle(newTitle.Data());
  PlotHisto(tend, gifName, kFALSE, kTRUE);

  newTitle = Form("Difference T_{End} - T_{Start} - %s Test - ", testName);
  gifName = Form("difftemp%s",testName);
  MakeTitles(newTitle, gifName, cond);

  difftemp->SetTitle(newTitle.Data());
  PlotHisto((TH1F*)difftemp->ProjectionY(), gifName, kFALSE, kTRUE);

  newTitle = Form("Temperature T_{Start} vs Site - %s Test - ", testName);
  gifName = Form("startempVsSite%s",testName);
  MakeTitles(newTitle, gifName, cond);

  starttemp->SetTitle(newTitle.Data());
  PlotHistoVsSite(starttemp, gifName, test);

  newTitle = Form("Temperature T_{End} vs Site - %s Test - ", testName);
  gifName = Form("endtempVsSite%s",testName);
  MakeTitles(newTitle, gifName, cond);

  endtemp->SetTitle(newTitle.Data());
  PlotHistoVsSite(endtemp, gifName, test);

  newTitle = Form("Difference T_{End} - T_{Start} vs Site - %s Test - ", testName);
  gifName = Form("difftempVsSite%s",testName);
  MakeTitles(newTitle, gifName, cond);

  difftemp->SetTitle(newTitle.Data());
  PlotHistoVsSite(difftemp, gifName, test);

  TLegend *siteLegendStart = new TLegend(0.6, 0.55, 0.82, 0.75);
  const Int_t siteOrderStart[numSites] = {2, 1, 3, 4, 5};
  gifName = Form("startempAllSites%s",testName);
  MakeTitles(newTitle, gifName, cond);

  PlotAllSites(starttemp, gifName, siteOrderStart, siteLegendStart, test);

  TLegend *siteLegendEnd = new TLegend(0.6, 0.55, 0.82, 0.75);
  const Int_t siteOrderEnd[numSites] = {2, 1, 3, 4, 5};
  gifName = Form("endtempAllSites%s",testName);
  MakeTitles(newTitle, gifName, cond);

  PlotAllSites(endtemp, gifName, siteOrderEnd, siteLegendEnd, test);

  TLegend *siteLegendDiff = new TLegend(0.6, 0.55, 0.82, 0.75);
  const Int_t siteOrderDiff[numSites] = {3, 1, 2, 4, 5};
  gifName = Form("difftempAllSites%s",testName);
  MakeTitles(newTitle, gifName, cond);

  PlotAllSites(difftemp, gifName, siteOrderDiff, siteLegendDiff, test);

  newTitle = Form("T_{Start} vs VDDA_{Start} - %s Test - ", testName);
  gifName = Form("tvddastart%s",testName);
  MakeTitles(newTitle, gifName, cond);

  for (Int_t i = 0; i<numSites; i++) {
    TString localNewTitle = newTitle + " - " + Site2Name(i+1,test);
    TString localGifName = Form("%s_%s", gifName.Data(), Site2Name(i+1,test).Data());

    tempVsVddaStart[i]->SetTitle(localNewTitle.Data());
    tempVsVddaStart[i]->GetXaxis()->SetTitle("VDDA_{Start} (V)");
    tempVsVddaStart[i]->GetYaxis()->SetTitle("Temp_{Start} (#circC)");

    PlotHisto(tempVsVddaStart[i], localGifName);
  }

  newTitle = Form("T_{End} vs VDDA_{End} - %s Test - ", testName);
  gifName = Form("tvddaend%s",testName);
  MakeTitles(newTitle, gifName, cond);

  for (Int_t i = 0; i<numSites; i++) {
    TString localNewTitle = newTitle + " - " + Site2Name(i+1,test);
    TString localGifName = Form("%s_%s", gifName.Data(), Site2Name(i+1,test).Data());

    tempVsVddaEnd[i]->SetTitle(localNewTitle.Data());
    tempVsVddaEnd[i]->GetXaxis()->SetTitle("VDDA_{End} (V)");
    tempVsVddaEnd[i]->GetYaxis()->SetTitle("Temp_{End} (#circC)");

    PlotHisto(tempVsVddaEnd[i], localGifName);
  }

  return;
}


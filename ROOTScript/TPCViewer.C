#include "TCanvas.h"
#include "TPolyLine.h"
#include "TH2Poly.h"
#include "TMath.h"
#include <iostream>
/*
TPC Parameter
#########################################################
#Layer# #OfPad Radaus   D(theta) theta  LengthOfPad
#########################################################
0   48  14.5    7.5 0.  9.
1   48  24. 7.5 0.  9.
2   72  33.5    5.  0.  9.
3   96  43. 3.75    0.  9.
4   120 52.5    3.  0.  9.
5   144 62. 2.5 0.  9.
6   168 71.5    2.14286 0.  9.
7   192 81. 1.875   0.  9.
8   216 90.5    1.66667 0.  9.
9   240 100.    1.5 0.  9.
10  208 111.25  1.49375 24.65   12.5
11  218 124.25  1.32844 35.2    12.5
12  230 137.25  1.2 42  12.5
13  214 150.25  1.09093 63.27   12.5
14  212 163.25  1.  74  12.5
15  214 176.25  0.923084    81.23   12.5
16  220 189.25  0.857182    85.71   12.5
17  224 202.25  0.801786    90.2    12.5
18  232 215.25  0.751552    92.82   12.5
19  238 228.25  0.707227    95.84   12.5
20  244 241.25  0.667869    98.52   12.5
21  232 254.25  0.632672    106.61  12.5
22  218 267.25  0.60101     114.49  12.5
23  210 280.25  0.573238    119.81  12.5
24  206 293.25  0.547111    123.648 12.5
25  202 306.25  0.523267    127.15  12.5
26  200 319.25  0.5014      129.86  12.5
27  196 332.25  0.481327    132.83  12.5
28  178 345.25  0.463371    138.76  12.5
29  130 358.25  0.446154    151 12.5
30  108 371.25  0.430185    156.77  12.5
31  90  384.25  0.415333    161.31  12.5
*/

const Double_t padParameter[32][6]={
  {0,   48, 14.5,   7.5 ,0. ,9.},
  {1,   48, 24.,    7.5 ,0. ,9.},
  {2,   72, 33.5,   5.  ,0. ,9.},
  {3,   96, 43.,    3.75    ,0. ,9.},
  {4,   120,    52.5,   3.  ,0. ,9.},
  {5,   144,    62.,    2.5 ,0. ,9.},
  {6,   168,    71.5,   2.14286 ,0. ,9.},
  {7,   192,    81.,    1.875   ,0. ,9.},
  {8,   216,    90.5,   1.66667 ,0. ,9.},
  {9,   240,    100.,   1.5 ,0. ,9.},
  {10,  208,    111.25, 1.49375 ,24.65  ,12.5},
  {11,  218,    124.25, 1.32844 ,35.2   ,12.5},
  {12,  230,    137.25, 1.2 ,42 ,12.5},
  {13,  214,    150.25, 1.09093 ,63.27  ,12.5},
  {14,  212,    163.25, 1.      ,74 ,12.5},
  {15,  214,    176.25, 0.923084,   81.23   ,12.5},
  {16,  220,    189.25, 0.857182,   85.71   ,12.5},
  {17,  224,    202.25, 0.801786,   90.2    ,12.5},
  {18,  232,    215.25, 0.751552,   92.82   ,12.5},
  {19,  238,    228.25, 0.707227,   95.84   ,12.5},
  {20,  244,    241.25, 0.667869,   98.52   ,12.5},
  {21,  232,    254.25, 0.632672,   106.61  ,12.5},
  {22,  218,    267.25, 0.60101 ,   114.49  ,12.5},
  {23,  210,    280.25, 0.573238,   119.81  ,12.5},
  {24,  206,    293.25, 0.547111,   123.648 ,12.5},
  {25,  202,    306.25, 0.523267,   127.15  ,12.5},
  {26,  200,    319.25, 0.5014  ,   129.86  ,12.5},
  {27,  196,    332.25, 0.481327,   132.83  ,12.5},
  {28,  178,    345.25, 0.463371,   138.76  ,12.5},
  {29,  130,    358.25, 0.446154,   151 ,12.5},
  {30,  108,    371.25, 0.430185,   156.77  ,12.5},
  {31,  90, 384.25, 0.415333,   161.31  ,12.5}};


TCanvas* can;
TPad* pad;
TPad* pad1;
TPad* pad2;
TH1D* hisTime[5768];
TH1D* hisHeight[5768];

void TPCViewer(){
  can = new TCanvas("can","can",1500,1000);
  for( int i = 0; i< 5768; i++){
    hisTime[i]   = new TH1D(Form("his_time_%d",i),Form("his_time_%d",i),100,0,100);
    hisHeight[i] = new TH1D(Form("his_height_%d",i),Form("his_height_%d",i),100,0,100);
  }

  double Offset = 11*11.5;
  pad  = new TPad("pad" ,"pad" ,0,0,1000./1500.,1);
  pad1 = new TPad("pad1","pad1",1000./1500.,0.5,1,1  );
  pad2 = new TPad("pad2","pad2",1000./1500.,0  ,1,0.5);
  pad->Draw();
  pad1->Draw();
  pad2->Draw();
  pad->cd();

  //TCanvas* can = new TCanvas("can","can",1000,1000);
  TH2Poly* poly = new TH2Poly("TPC","TPC",-300+Offset,300+Offset,-300,300);
  TPolyLine* p[5768];
  Int_t nPannel[32]={24,24,36,48,60,
             72,84,96,108,120,
             104,109,115,107,106,
             107,110,112,116,119,
             122,116,109,105,103,
             101,100,98,89,65,
             54,45};

  Double_t MaximumAngle[32]={1,1,1,1,1,
                 1,1,1,1,1,
                 0.85,0.8 ,0.75,0.65,0.6,
                 0.55,0.52,0.5 ,0.48,0.45,
                 0.43,0.4 ,0.38,0.35,0.33,
                 0.31,0.29,0.27,0.25,0.15,
                 0.12,0.1};

  double l = 260./(1+sqrt(2.));
  Double_t px[9]={-l*(1+sqrt(2.)),-l,l,l*(1+sqrt(2.)),
          l*(1+sqrt(2.)),l,-l,-l*(1+sqrt(2.)),
          -l*(1+sqrt(2.))};
  Double_t py[9]={l,l*(1+sqrt(2.)),l*(1+sqrt(2.)),l,
          -l,-l*(1+sqrt(2.)),-l*(1+sqrt(2.)),-l,
          l};
  TPolyLine* pLine= new TPolyLine(9,px,py);
  pLine->SetLineColor(2);


  int npoint = 5;
  double x[5];
  double y[5];
  int npl = 0;

  //CenterOriginVersion//
  /*
  for( int i = 0; i< 32; i++){
    double pLength = padParameter[i][5];
    double sTheta  = padParameter[i][4]/180.*TMath::Pi();
    double dTheta  = padParameter[i][3]/180.*TMath::Pi();
    double cRad    = padParameter[i][2];
    int    nPad    = padParameter[i][1];

    for( int j = 0; j< nPad/2; j++){
      x[1] = (cRad+(pLength/2.))*cos(j*dTheta);
      x[2] = (cRad+(pLength/2.))*cos((j+1)*dTheta);
      x[3] = (cRad-(pLength/2.))*cos((j+1)*dTheta);
      x[4] = (cRad-(pLength/2.))*cos(j*dTheta);
      x[0] = x[4];
      y[1] = (cRad+(pLength/2.))*sin(j*dTheta);
      y[2] = (cRad+(pLength/2.))*sin((j+1)*dTheta);
      y[3] = (cRad-(pLength/2.))*sin((j+1)*dTheta);
      y[4] = (cRad-(pLength/2.))*sin(j*dTheta);
      y[0] = y[4];
      poly->AddBin(5,x,y);
      p[npl] = new TPolyLine(5,x,y);
      p[npl]->SetUniqueID(npl+1);
      npl++;
    }
    for( int j = 0; j< nPad/2; j++){
      x[1] = (cRad+(pLength/2.))*cos(j*dTheta);
      x[2] = (cRad+(pLength/2.))*cos((j+1)*dTheta);
      x[3] = (cRad-(pLength/2.))*cos((j+1)*dTheta);
      x[4] = (cRad-(pLength/2.))*cos(j*dTheta);
      x[0] = x[4];
      y[1] = -(cRad+(pLength/2.))*sin(j*dTheta);
      y[2] = -(cRad+(pLength/2.))*sin((j+1)*dTheta);
      y[3] = -(cRad-(pLength/2.))*sin((j+1)*dTheta);
      y[4] = -(cRad-(pLength/2.))*sin(j*dTheta);
      y[0] = y[4];
      poly->AddBin(5,x,y);
      p[npl] = new TPolyLine(5,x,y);
      p[npl]->SetUniqueID(npl+1);
      npl++;
    }
  }
  */

  //EdgeOriginVersion -AntiClockwise
  for( int i = 0; i< 32; i++){
    double pLength = padParameter[i][5];
    double sTheta  = (-1+padParameter[i][4]/180.)*TMath::Pi();
    double dTheta  = padParameter[i][3]/180.*TMath::Pi();
    double cRad    = padParameter[i][2];
    int    nPad    = padParameter[i][1];
    for( int j = 0; j< nPad; j++){
      x[1] = (cRad+(pLength/2.))*cos(j*dTheta+sTheta);
      x[2] = (cRad+(pLength/2.))*cos((j+1)*dTheta+sTheta);
      x[3] = (cRad-(pLength/2.))*cos((j+1)*dTheta+sTheta);
      x[4] = (cRad-(pLength/2.))*cos(j*dTheta+sTheta);
      x[0] = x[4];
      y[1] = (cRad+(pLength/2.))*sin(j*dTheta+sTheta);
      y[2] = (cRad+(pLength/2.))*sin((j+1)*dTheta+sTheta);
      y[3] = (cRad-(pLength/2.))*sin((j+1)*dTheta+sTheta);
      y[4] = (cRad-(pLength/2.))*sin(j*dTheta+sTheta);
      y[0] = y[4];
      poly->AddBin(5,x,y);
      p[npl] = new TPolyLine(5,x,y);
      p[npl]->SetUniqueID(npl+1);
      npl++;
    }
  }


  /*
  int idraw = 0;
  int idcol = 23;
  int padID = DIDtoPadID( idraw, idcol );
  poly->SetBinContent( padID, 100);
  idraw = 33;
  idcol = 23;
  padID = DIDtoPadID( idraw, idcol );
  poly->SetBinContent( padID, 100);
  std::cout << poly->Fill(230,115) << std::endl;
  std::cout << poly->Fill(230,-115) << std::endl;
  */


  poly->Draw("colz");
  //pLine->Draw("same");
  for( int i = 0; i< 5768; i++){
    p[i]->Draw("same");
  }

  std::cout << "Ex" << std::endl;
  pad->AddExec("ex","BinClicked()");
  std::cout << "Ex" << std::endl;
}

void BinClicked(){
  int event = pad->GetEvent();
  //int x = pad->GetX();
  //int y = pad->GetY();
  //std::cout<< x << "\t" << y << std::endl;
  if( event != 11 ) return;
  TObject* select = gPad->GetSelected();
  TPolyLine* pl;
  if(!select) return;
  if( select->InheritsFrom(TPolyLine::Class())){
    pl = (TPolyLine*)select;
    printf("PLID: %d\n",pl->GetUniqueID());
  }else{ return;}
  int id = pl->GetUniqueID();
  pad1->cd();
  hisTime[id-1]->Draw();
  pad2->cd();
  hisHeight[id-1]->Draw();
  can->Modified();
  can->Update();
}


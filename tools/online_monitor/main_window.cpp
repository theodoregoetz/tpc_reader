#include "TApplication.h"
#include "TGClient.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TRandom.h"
#include "TGButton.h"

#include "main_window.hpp"

MainWindow::MainWindow(const TGWindow *p,UInt_t w,UInt_t h)
: TGMainFrame(p,w,h)
, fEcanvas("Ecanvas",this,200,200)
{
    AddFrame(&fEcanvas, new TGLayoutHints(kLHintsExpandX |
                    kLHintsExpandY, 10,10,10,1));
    TGHorizontalFrame *hframe = new TGHorizontalFrame(this,200,40);
    TGTextButton *draw = new TGTextButton(hframe,"&Draw");
    draw->Connect("Clicked()","MainWindow",this,"DoDraw()");
    hframe->AddFrame(draw, new TGLayoutHints(kLHintsCenterX,
                     5,5,3,4));
    TGTextButton *exit = new TGTextButton(hframe,"&Exit",
                                          "gApplication->Terminate(0)");
    hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,
                     5,5,3,4));
    AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,
                    2,2,2,2));
    SetWindowName("Simple Example");
    MapSubwindows();
    Resize(GetDefaultSize());
    MapWindow();
}

void MainWindow::DoDraw() {
    // Draws function graphics in randomly chosen interval
    TF1* f1 = new TF1("f1","sin(x)/x",0,gRandom->Rndm()*10);
    f1->SetLineWidth(3);
    f1->Draw();
    TCanvas* fCanvas = fEcanvas.GetCanvas();
    fCanvas->cd();
    fCanvas->Update();
}

MainWindow::~MainWindow() {
    // Clean up used widgets: frames, buttons, layout hints
    Cleanup();
}

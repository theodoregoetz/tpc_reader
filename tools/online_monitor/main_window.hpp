#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"

class MainWindow : public TGMainFrame
{
  private:
    TRootEmbeddedCanvas fEcanvas;

  public:
    MainWindow(const TGWindow *p,UInt_t w,UInt_t h);
    virtual ~MainWindow();

    void DoDraw();

    ClassDef(MainWindow,0)
};

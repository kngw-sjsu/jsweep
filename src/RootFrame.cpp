//
// wxFrame derived class that contains the main 'panel' of the app, and menubar.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/menu.h>
#include <wx/sizer.h>
#include "RootFrame.h"
#include "MainPanel.h"
#include "MenuBar.h"
#include "dialog/NewGameDialog.h"


/*************************************/
/* Definitions                       */
/*************************************/

RootFrame::RootFrame(States & states) :
    wxFrame(nullptr, wxID_ANY, "jSweeper", wxDefaultPosition, wxDefaultSize,
            wxDEFAULT_FRAME_STYLE ^ wxRESIZE_BORDER ^ wxMAXIMIZE_BOX) {
    SetIcon(wxICON(aaaaaa));
    SetMenuBar(new MenuBar(this, states));
    new MainPanel(this, states); // MainPanel's size is all calculated within its class.
    // The instance is also freed automatically by the library when the program exits.
}

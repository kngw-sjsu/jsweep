//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_MAINPANEL_H
#define JSWEEP_MAINPANEL_H

#include "common.h"
#include "DigitalFontSet.h"

class MainPanel : public wxPanel {
    static const wxInt32
        NUMBER_WINDOW_WIDTH,
        NUMBER_WINDOW_HEIGHT;
    wxInt32 timeElapsed;
    DigitalFontSet
        mineNumFontSet,
        timerFontSet;
public:
    MainPanel(wxWindow* parent, States & states);
    ~MainPanel(){ PRINT_MSG("MainPanel destr called");}
};


#endif //JSWEEP_MAINPANEL_H

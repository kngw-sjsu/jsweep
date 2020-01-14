//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_CHARACTEREDITORDIALOG_H
#define JSWEEP_CHARACTEREDITORDIALOG_H

#include "common.h"

#include <wx/dialog.h>

class CharacterEditorDialog : public wxDialog {
public:
    CharacterEditorDialog(wxWindow* parent, States & states);
};


#endif //JSWEEP_CHARACTEREDITORDIALOG_H

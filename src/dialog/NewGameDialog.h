//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_NEWGAMEDIALOG_H
#define JSWEEP_NEWGAMEDIALOG_H

#include "common.h"

#include <wx/dialog.h>
#include <wx/textctrl.h>
#include "DigitalFontSet.h"

class NewGameDialog : public wxDialog {
    class PercentageText : public wxWindow {
        DigitalFontSet fontSet;
        wxUint32 value, range, mid;
        static const int FONT_SIZE;
    protected:
        wxSize DoGetBestClientSize() const override;
    public:
        PercentageText(wxWindow* parent, wxUint32 lower, wxUint32 upper, wxUint32 ratio);
        int GetValue() {
            return value;
        }
        void SetValue(wxUint32 value) {
            this->value = value;
        }
    } *percentText;
    wxTextCtrl *widthTextCtrl, *heightTextCtrl;
public:
    explicit NewGameDialog(wxWindow* frame, States & states);
    wxUint32 GetRequestedHeight() {
        double d;
        heightTextCtrl->GetLineText(0).ToDouble(&d);
        return (wxUint32) d;
    }
    wxUint32 GetRequestedWidth() {
        double d;
        widthTextCtrl->GetLineText(0).ToDouble(&d);
        return (wxUint32) d;
    }
    wxUint32 GetRequestedRatio() {
        return percentText->GetValue();
    }
};


#endif //JSWEEP_NEWGAMEDIALOG_H

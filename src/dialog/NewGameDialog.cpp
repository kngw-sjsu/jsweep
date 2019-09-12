//
// Opens the dialog where user inputs the size of the minefield and the mine to map ratio.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/button.h>
#include <wx/dcbuffer.h>
#include <wx/gbsizer.h>
#include <wx/graphics.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include "NewGameDialog.h"


/*************************************/
/* Definitions                       */
/*************************************/

NewGameDialog::NewGameDialog(wxWindow* frame)
    : wxDialog(frame, wxID_ANY, "Start New Game", wxDefaultPosition, wxDefaultSize) {
    auto topSizer = new wxBoxSizer(wxVERTICAL);

    // for upper area: anything except buttons
    auto upperSizer = new wxBoxSizer(wxHORIZONTAL);

    // for dimensions settings box
    auto dimensionsSettingsBox = new wxStaticBox(this, wxID_ANY, wxString::FromUTF8("Dimensions"));
    auto dsBoxSizer = new wxStaticBoxSizer(dimensionsSettingsBox, wxVERTICAL);
    upperSizer->Add(dsBoxSizer, 0, wxRIGHT | wxEXPAND, 5); // 横に5pxスペースを空ける

    // for text fields to specify dimensions
    auto textInputSizer = new wxGridBagSizer(2, 2); // 子ノードに縦横共に2pxの余白を持たせたgbsizer
    textInputSizer->Add(new wxStaticText(dimensionsSettingsBox, wxID_ANY, wxString::FromUTF8("Width:"),
            wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE),
                    wxGBPosition(0,0), wxDefaultSpan, wxEXPAND); // POSSIBILITY OF BAD LAYOUT! MUST EDIT
    widthTextCtrl = new wxTextCtrl(dimensionsSettingsBox, wxID_ANY, wxString::FromUTF8("12"),
            wxDefaultPosition, wxSize(32,-1));
    textInputSizer->Add(widthTextCtrl, wxGBPosition(0,1)); // POSSIBILITY OF BAD LAYOUT! MUST EDIT
    textInputSizer->Add(new wxStaticText(dimensionsSettingsBox, wxID_ANY, wxString::FromUTF8("Height:"),
            wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE),
                    wxGBPosition(1,0), wxDefaultSpan, wxEXPAND); // POSSIBILITY OF BAD LAYOUT! MUST EDIT
    heightTextCtrl = new wxTextCtrl(dimensionsSettingsBox, wxID_ANY, wxString::FromUTF8("9"),
            wxDefaultPosition, wxSize(32,-1));
    textInputSizer->Add(heightTextCtrl, wxGBPosition(1,1)); // POSSIBILITY OF BAD LAYOUT! MUST EDIT
    dsBoxSizer->Add(textInputSizer);

    // for slider box
    auto sliderBox = new wxStaticBox(this, wxID_ANY, wxString::FromUTF8("Mine:Map Ratio"));
    auto sliderBoxSizer = new wxStaticBoxSizer(sliderBox, wxHORIZONTAL);
    upperSizer->Add(sliderBoxSizer);
    percentText = new PercentageText(sliderBox, 5, 75); // digital stuff
    sliderBoxSizer->Add(percentText, 0, wxALIGN_TOP);
    auto slider = new wxSlider(sliderBox, wxID_ANY, 15, 5, 75, wxDefaultPosition,
            wxDefaultSize, wxSL_VERTICAL | wxSL_MIN_MAX_LABELS | wxSL_INVERSE); // the slider
    // to force redraw text if slider pos changed
    slider->Bind(wxEVT_SLIDER, [this](wxCommandEvent & event) {
        percentText->SetValue((wxUint32)event.GetInt());
        percentText->Refresh(); // force redraw
    });
    sliderBoxSizer->Add(slider);

    // Finally add everything in upper area to the topmost sizer
    topSizer->Add(upperSizer, 0, wxTOP | wxRIGHT | wxLEFT | wxEXPAND, 5);

    // for lower area: buttons
    auto lowerSizer = new wxBoxSizer(wxHORIZONTAL);
    auto cancelButton = new wxButton(this, wxID_CANCEL, wxEmptyString, wxDefaultPosition,
            wxDefaultSize, wxBU_EXACTFIT);
    lowerSizer->Add(cancelButton, 0, wxRIGHT, 5);
    auto startButton = new wxButton(this, wxID_OK, wxString::FromUTF8("Start"), wxDefaultPosition,
            wxDefaultSize, wxBU_EXACTFIT);
    lowerSizer->Add(startButton, 0);
    topSizer->Add(lowerSizer, 0, wxALL | wxALIGN_RIGHT, 5);
    SetSizerAndFit(topSizer); // pack everything
}

/* PercentageText inner class */
const int NewGameDialog::PercentageText::FONT_SIZE = 23;

NewGameDialog::PercentageText::PercentageText(wxWindow* parent, wxUint32 lower, wxUint32 upper)
        : wxWindow(parent, wxID_ANY, wxDefaultPosition, wxSize(48, 20)), value(15), range((upper - lower) / 2),
        mid(range + lower) {
    SetBackgroundStyle(wxBG_STYLE_PAINT); // to use wxAutoBufferedPaintDC
    Bind(wxEVT_PAINT, [this, upper, lower](wxPaintEvent & event) {
        wxAutoBufferedPaintDC dc(this); // this = PercentageText
        dc.Clear();
        if (auto context = wxGraphicsContext::Create(dc)) {
            wxUint8 r, g, b;
            // wxColour fontColor;
            if (lower <= value && value < mid) { // value = Current position
                r = (wxUint32) (((value - lower) / ((double)range)) * 255.0);
                g = 0;
                b = 255;
            }
            else if (value == mid) { // Exactly middle
                r = 255;
                g = 0;
                b = 255;
            }
            else { // mid ~ upper
                r = 255;
                g = 0;
                b = (wxUint32) (255 - (((value - mid) / ((double)range)) * 255.0));
            }
            fontSet.SetColor(r, g, b);
            auto
                & d1 = fontSet.GetTextImage(wxString::Format("%d", value % 10)),
                & d10 = fontSet.GetTextImage(wxString::Format("%d", value / 10)),
                & perc = fontSet.GetTextImage("perc");
            context->DrawBitmap(wxBitmap(d10), 0, 0, d10.GetWidth(), d10.GetHeight());
            context->DrawBitmap(wxBitmap(d1), d10.GetWidth(), 0, d1.GetWidth(), d1.GetHeight());
            context->DrawBitmap(wxBitmap(perc), d10.GetWidth() + d1.GetWidth(), 0,
                perc.GetWidth(), perc.GetHeight());
            delete context;
        }
    });
}

wxSize NewGameDialog::PercentageText::DoGetBestClientSize() const {
    return wxSize((FONT_SIZE/2)*4,(int)(FONT_SIZE*1.2));
}

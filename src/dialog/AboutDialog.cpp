//
// Just a simple dialog with info on the version, credits, and stuff.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/graphics.h>
#include <wx/stattext.h>
#include "appinfo.h"
#include "AboutDialog.h"


/*************************************/
/* Definitions                       */
/*************************************/

AboutDialog::AboutDialog(wxWindow* frame) : wxDialog(frame, wxID_ANY, "About jSweeper") {
    auto topSizer = new wxBoxSizer(wxVERTICAL);
    auto sizer = new wxBoxSizer(wxVERTICAL);

    auto lrSizer = new wxBoxSizer(wxHORIZONTAL);

    // Draw the icon on the left side of the frame
    auto imagePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(128, 128));
    imagePanel->Bind(wxEVT_PAINT, [imagePanel](wxPaintEvent & event) {
        if (auto context = wxGraphicsContext::Create(wxPaintDC(imagePanel))) {
            context->DrawBitmap(wxBITMAP_PNG(about_icon), 0.0, 0.0, 128, 128);
            delete context;
        }
    });

    auto textSizer = new wxBoxSizer(wxVERTICAL);

    // Draw the name of the app with specified font and size.
    auto appName = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(200, 30));
    appName->Bind(wxEVT_PAINT, [appName](wxPaintEvent & event) {
        if (auto context = wxGraphicsContext::Create(wxPaintDC(appName))) {
            context->SetFont(wxFont(wxFontInfo(20)), wxColour(117, 9, 9));
            context->DrawText("jSweeper", 0.0, 0.0);
            delete context;
        }
    });
    textSizer->Add(appName, 0, wxBOTTOM, 6);
    textSizer->Add(new wxStaticText(this, wxID_ANY, wxString::Format("Version: %s", AppInfo::GetVersion())), 0, wxBOTTOM, 3);
    textSizer->Add(new wxStaticText(this, wxID_ANY, wxString::Format("Developed by %s",
        AppInfo::GetAuthor())), 0, wxBOTTOM, 3);
    wxString str = "";
    auto usedResources = AppInfo::GetUsedResources();
    std::for_each(usedResources.begin(), usedResources.end(),
        [&str, &usedResources](const wxString & resource){str << wxString::Format("  %s%s",
            resource, resource != *(usedResources.end() - 1) ? "\n" : "" );});
    textSizer->Add(new wxStaticText(this, wxID_ANY, wxString::Format("Resources used:\n%s", str)));

    lrSizer->Add(imagePanel, 0, wxRIGHT, 5);
    lrSizer->Add(textSizer);

    sizer->Add(lrSizer);
    sizer->Add(new wxButton(this, wxID_OK, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT),
        0, wxALIGN_RIGHT);

    topSizer->Add(sizer, 0, wxALL, 5);

    SetSizerAndFit(topSizer);
}

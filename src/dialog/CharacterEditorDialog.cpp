//
// TO DO: Remove existing PNGs under the img folder in the case of updating existing characters.
//

#include <wx/button.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <iomanip>
#include "CharacterEditorDialog.h"

#include "state/AvailableCharactersState.h"
#include "state/CharacterState.h"

/*************************************/
/* Definitions                       */
/*************************************/

// Helper functions that need to not be accessed from elsewhere go here
namespace {
    // Determines if the character with the entered name already exists in JSON
    bool characterExists(const wxString & characterName, const AvailableCharactersState & availableCharactersState) {
        for (auto & entry : availableCharactersState.GetAllCharacters()) {
            if (characterName.IsSameAs(entry.name))
                return true;
        }
        return false;
    }
}

CharacterEditorDialog::CharacterEditorDialog(wxWindow *parent, States & states) :
        wxDialog(parent, wxID_ANY, "Add New Character") {
    // These are the prefixes that get appended to each image file stored under "img"
    const static std::array<wxString, 4> imgNamePrefixes4States = {
            "-normal",
            "-leftdown",
            "-win",
            "-lose"
    };

    // These strings are to be used in the for loop in the constructor
    // ...to avoid having to create 4 distinct blocks of nearly same code
    const static std::array<std::array<wxString, 2>, 4> stateStaticText = {
            "Normal State", "Select Image For Normal State",
            "Left-Down State", "Select Image For Left-Down State",
            "Win State", "Select Image For Win State",
            "Lose State", "Select Image For Lose State"
    };

    auto & availableCharactersState = std::get<AvailableCharactersState &>(states);
    auto & characterState = std::get<CharacterState &>(states);

    auto sizer = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* statePathTexts[5]; // This array holds pointers to the textctrls which will be referenced to later on

    // Character name box
    auto characterNameStaticBox = new wxStaticBox(this, wxID_ANY, "Character Name");
    auto characterNameTextCtrl = new wxTextCtrl(characterNameStaticBox, wxID_ANY);
    statePathTexts[0] = characterNameTextCtrl;
    auto characterNameSizer = new wxStaticBoxSizer(characterNameStaticBox, wxVERTICAL);
    characterNameSizer->Add(characterNameTextCtrl, 0, wxEXPAND);
    sizer->Add(characterNameSizer, 0, wxALL | wxEXPAND, 5);

    sizer->Add(new wxStaticLine(this), 0, wxRIGHT | wxLEFT | wxEXPAND, 5);

    // "Images:" static text
    auto imagesText = new wxStaticText(this, wxID_ANY, "Images:");
    sizer->Add(imagesText, 0, wxTOP | wxLEFT | wxRIGHT | wxALIGN_LEFT, 5);

    /* 4 state image selection boxes
     * [0]: Normal
     * [1]: Left-Down
     * [2]: Win
     * [3]: Lose
     */
    for (wxUint32  i = 0; i < stateStaticText.size(); ++i) {
        auto stateStaticBox = new wxStaticBox(this, wxID_ANY, stateStaticText[i][0]);
        auto stateSizer = new wxStaticBoxSizer(stateStaticBox, wxHORIZONTAL);
        auto statePathText = new wxTextCtrl(stateStaticBox, wxID_ANY);
        statePathTexts[i + 1] = statePathText;
        auto stateBrowseButton = new wxButton(stateStaticBox, wxID_ANY, "Browse", wxDefaultPosition,
                wxDefaultSize, wxBU_EXACTFIT);
        stateBrowseButton->Bind(wxEVT_BUTTON, [this, i, statePathText](wxCommandEvent & event) {
            wxFileDialog dlg(this, stateStaticText[i][1], wxEmptyString, wxEmptyString,
                             "PNG files (*.png)|*.png");
            if (dlg.ShowModal() == wxID_OK) {
                statePathText->ChangeValue(dlg.GetPath());
            }
        });
        stateSizer->Add(statePathText, 1, wxRIGHT, 3);
        stateSizer->Add(stateBrowseButton);
        sizer->Add(stateSizer, 0, wxTOP | wxLEFT | wxRIGHT | wxEXPAND, 5);
    }

    // Import note static text
    auto noteStaticText = new wxStaticText(this, wxID_ANY, wxString::FromUTF8(
            "・Images will be copied to /img folder.\n"
            "・Only PNG format is currently supported.\n"
            "・It is advised that the width and height share the same length."
            ), wxDefaultPosition, wxSize(250, 78));
    sizer->Add(noteStaticText, 0, wxALL | wxEXPAND, 5);

    sizer->Add(new wxStaticLine(this), 0, wxRIGHT | wxLEFT | wxEXPAND, 5);

    auto buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    auto cancelButton = new wxButton(this, wxID_CANCEL, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    auto okButton = new wxButton(this, wxID_OK, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    okButton->Bind(wxEVT_BUTTON, [this, &characterState, &availableCharactersState, statePathTexts](wxCommandEvent & event) {
        /*nlohmann::json j(nullptr);
        // Load the json file, if it already exists
        if (wxFileExists("config.json")) {
            std::ifstream ifs("config.json");
            ifs >> j;
        }*/
        // Bitmaps to be used later on when adding the character to the array in the CharacterState.
        std::array<wxBitmap, 4> bitmaps;

        for (wxUint32  i = 0; i < 5; ++i) {
            // Stop, if at least one field is empty
            if (statePathTexts[i]->IsEmpty()) {
                wxMessageDialog errorDlg(this, "Please fill in all fields with appropriate paths.",
                        "Empty Field Detected!", wxOK | wxOK_DEFAULT | wxCENTER | wxICON_EXCLAMATION);
                errorDlg.ShowModal();
                return;
            }

            // Stop, if the name entered already exists in json.
            if (!i && characterExists(statePathTexts[0]->GetLineText(0), availableCharactersState)) {
                wxMessageDialog errorDlg(this,
                        "Invalid input. Please make sure the character name is unique.",
                        "Invalid Input!", wxOK | wxOK_DEFAULT | wxCENTER | wxICON_EXCLAMATION);
                errorDlg.ShowModal();
                return;
            } else if (i) {
                wxImage image;
                // Stop, if the image pointed to by the field is of invalid format.
                if (!image.LoadFile(statePathTexts[i]->GetLineText(0), wxBITMAP_TYPE_PNG)) {
                    wxMessageDialog errorDlg(this,
                        "One or more of the selected images are not of valid format!\nPlease select a valid image file.",
                        "Invalid Input!", wxOK | wxOK_DEFAULT | wxCENTER | wxICON_EXCLAMATION);
                    errorDlg.ShowModal();
                    return;
                }
                // If the folder "img" does not exist, create one
                if (!wxDirExists("img"))
                    wxMkdir("img");
                // Rescale the image so it is 52x52 (Required size), then save it to the img folder.
                image.Rescale(CharacterState::CHARACTER_IMAGE_LENGTH,CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE)
                    .SaveFile(wxString::Format("img/%s%s.png", statePathTexts[0]->GetLineText(0), imgNamePrefixes4States[i - 1]));
                // Convert the wxImage to wxBitmap, as it will be stored internally into AvailableCharactersState.
                bitmaps[i - 1] = wxBitmap(image);
            }
        }

        // CREATE JSON HERE!!!!
        // j["additional_chars"].push_back({statePathTexts[0]->GetLineText(0)});

        // std::ofstream ofs("config.json");
        // ofs << std::setw(4) << j << std::endl;

        // Finally, add the bitmaps and their original paths used for this character to the array in AvailableCharactersState.
        availableCharactersState.AddCharacter(statePathTexts[0]->GetLineText(0), bitmaps);
        PRINT_MSG("Reached");
        availableCharactersState.NotifyAll();

        // call this if everything went ok
        event.Skip();
    });
    buttonSizer->Add(cancelButton, 0, wxRIGHT, 5);
    buttonSizer->Add(okButton);
    sizer->Add(buttonSizer, 0, wxALL | wxALIGN_RIGHT, 5);

    SetSizerAndFit(sizer);
}

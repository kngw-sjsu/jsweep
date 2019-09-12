//
// kngw-sjsu (c) 2019
// Not the main function, but pretty much the "entry point" of the app.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/imagpng.h>
#include "RootFrame.h"
#include "SweeperApp.h"


/*************************************/
/* Definitions                       */
/*************************************/

bool SweeperApp::OnInit() {
    // Initialize PNG handler so the program can accept PNG files
    wxImage::AddHandler(new wxPNGHandler); // Will be deleted automatically upon program exit

    // PRINT_MSG(wxFont::AddPrivateFont(FONT_PATH));

    /* Load the default character. This will always be the first character in the
     * ...internal vector "characterBitmaps" in CharacterState class
     * Because RC resources are not available before this class is instantiated,
     * ...the character has to be added separately here
     * Removing this is not a good idea.
     */
    availableCharactersState.AddCharacter("Carlos", {
        std::pair<wxBitmap, wxString>{
            wxBitmap(wxBITMAP_PNG(carlos_normal).ConvertToImage().Scale(CharacterState::CHARACTER_IMAGE_LENGTH,
                CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE)), ""
                },
        std::pair<wxBitmap, wxString>{wxBitmap(wxBITMAP_PNG(carlos_left_down).ConvertToImage().Scale(CharacterState::CHARACTER_IMAGE_LENGTH,
                CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE)), ""
                },
        std::pair<wxBitmap, wxString>{wxBitmap(wxBITMAP_PNG(carlos_win).ConvertToImage().Scale(CharacterState::CHARACTER_IMAGE_LENGTH,
                CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE)), ""
                },
        std::pair<wxBitmap, wxString>{wxBitmap(wxBITMAP_PNG(carlos_lose).ConvertToImage().Scale(CharacterState::CHARACTER_IMAGE_LENGTH,
                CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE)), ""
        }
    });

    // Load custom characters, if any exists.
    // No scaling is done here because custom characters are already scaled to the required size (52x52).
    if (wxFileExists("additional_chars.json")) {
        std::ifstream ifs("additional_chars.json");
        nlohmann::json j;
        ifs >> j;
        for (auto & entry : j) {
            auto name = entry["name"].get<std::string>();
            availableCharactersState.AddCharacter(name, {
                std::pair<wxBitmap, wxString>{
                    wxBitmap(wxString::Format("img/%s-normal.png", name), wxBITMAP_TYPE_PNG),
                    entry["origImgPaths"]["normal"].get<std::string>()
                },
                std::pair<wxBitmap, wxString>{
                    wxBitmap(wxString::Format("img/%s-leftdown.png", name), wxBITMAP_TYPE_PNG),
                    entry["origImgPaths"]["leftdown"].get<std::string>()
                },
                std::pair<wxBitmap, wxString>{
                    wxBitmap(wxString::Format("img/%s-win.png", name), wxBITMAP_TYPE_PNG),
                    entry["origImgPaths"]["win"].get<std::string>()
                },
                std::pair<wxBitmap, wxString>{
                    wxBitmap(wxString::Format("img/%s-lose.png", name), wxBITMAP_TYPE_PNG),
                    entry["origImgPaths"]["lose"].get<std::string>()
                }
            });
        }
    }

    // Construction!
    auto frame = new RootFrame(states);
    frame->Show();
    PRINT_MSG(wxGetCwd());
    return true;
}

wxIMPLEMENT_APP(SweeperApp);

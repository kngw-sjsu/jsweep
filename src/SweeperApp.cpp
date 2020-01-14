//
// kngw-sjsu (c) 2019
// Not the main function, but pretty much the "entry point" of the app.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <iomanip>
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
     * Once the character has been added, update current character to the default one.
     * The current character may be updated later on in case the last selected character was a different one.
     */
    characterState.ChangeCharacter(availableCharactersState.AddCharacter("Carlos", {
        wxBitmap(wxBITMAP_PNG(carlos_normal).ConvertToImage().Scale(CharacterState::CHARACTER_IMAGE_LENGTH,
            CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE)),
        wxBitmap(wxBITMAP_PNG(carlos_left_down).ConvertToImage().Scale(CharacterState::CHARACTER_IMAGE_LENGTH,
            CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE)),
        wxBitmap(wxBITMAP_PNG(carlos_win).ConvertToImage().Scale(CharacterState::CHARACTER_IMAGE_LENGTH,
            CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE)),
        wxBitmap(wxBITMAP_PNG(carlos_lose).ConvertToImage().Scale(CharacterState::CHARACTER_IMAGE_LENGTH,
            CharacterState::CHARACTER_IMAGE_LENGTH, wxIMAGE_QUALITY_BOX_AVERAGE))
    }));

    // If not the first time running this application, load all the settings from the last round
    if (wxFileExists("config.json")) {
        std::ifstream ifs("config.json");
        nlohmann::json j;
        ifs >> j;
        gameState.SetMapInfo(j["map"]["w"].get<wxInt32>(),
            j["map"]["h"].get<wxInt32>(), j["map"]["ratio"].get<wxInt32>());
        auto lastSelectedCharacter = j["last_selected_char"].get<std::string>();

        // Load custom characters, if any exists.
        // No scaling is done here because custom characters are already scaled to the required size (52x52).
        if (j.contains("additional_chars")) {
            for (auto &entry : j["additional_chars"]) {
                wxString name = entry.get<std::string>();
                auto & thisCharacter = availableCharactersState.AddCharacter(name, {
                        wxBitmap(wxString::Format("img/%s-normal.png", name), wxBITMAP_TYPE_PNG),
                        wxBitmap(wxString::Format("img/%s-leftdown.png", name), wxBITMAP_TYPE_PNG),
                        wxBitmap(wxString::Format("img/%s-win.png", name), wxBITMAP_TYPE_PNG),
                        wxBitmap(wxString::Format("img/%s-lose.png", name), wxBITMAP_TYPE_PNG),
                });
                // If this character that was just added right now is the last character displayed
                //      when the app was shut down the last time (as saved in config.json), make it the current character.
                PRINT_MSG("thisCharacter: " << &thisCharacter);
                if (thisCharacter.name == lastSelectedCharacter)
                    characterState.ChangeCharacter(thisCharacter);
                PRINT_MSG("characterState.GetCurrentCharacter: " << &characterState.GetCurrentCharacter());
            }
        }
    }

    // Construction!
    auto frame = new RootFrame(states);
    frame->Show();
    PRINT_MSG(wxGetCwd());
    return true;
}

int SweeperApp::OnExit() {
    nlohmann::json j;
    // config.json not existing typically means it is your first time to ever close this application after installation.
    if (wxFileExists("config.json")) {
        std::ifstream ifs("config.json");
        ifs >> j;
    }
    if (j.contains("additional_chars"))
        j["additional_chars"].clear();
    auto & allCharacters = availableCharactersState.GetAllCharacters();
    // Since we do not want to add the default character info to the file, get the second element as the first below
    for (auto characterIt = ++allCharacters.begin(); characterIt != allCharacters.end(); ++characterIt)
        j["additional_chars"].emplace_back((*characterIt).name);
    auto mapSize = gameState.GetSize();
    j["map"] = {
            {"w", mapSize.x},
            {"h", mapSize.y},
            {"ratio", gameState.GetRatio()}
    };
    j["last_selected_char"] = characterState.GetCurrentCharacter().name;
    std::ofstream ofs("config.json");
    ofs << std::setw(4) << j << std::endl;
    return 0;
}

wxIMPLEMENT_APP(SweeperApp);

//
// The menubar that is used for a lot of purposes.
// Probably starting the game is all that really matters in this class..
// TO DO: Complete implementation of all menu items.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/msgdlg.h>
#include "MenuBar.h"
#include "dialog/AboutDialog.h"
#include "dialog/CharacterEditorDialog.h"
#include "dialog/NewGameDialog.h"


/*************************************/
/* Definitions                       */
/*************************************/

MenuBar::MenuBar(wxWindow* frame, States & states) :
        wxMenuBar(), promptUponNewGame(false), characterRadioItemNum(0) {
    // 'Extract' the states that will be used in this class
    auto & availableCharactersState = std::get<AvailableCharactersState &>(states);
    auto & characterState = std::get<CharacterState &>(states);
    auto & gameState = std::get<GameState &>(states);


    // Register. The function passed is what gets called upon notify all (Basically "update" function)
    gameState.Register([this](const GameState & gameState){
        promptUponNewGame = gameState.IsInGame();
    });

    // Add "File" menu
    auto file = new wxMenu;
    file->Append(wxID_ANY, wxString::FromUTF8("Load View Settings"));
    file->Append(wxID_ANY, wxString::FromUTF8("Save Current View Settings"));
    file->AppendSeparator();
    file->Append(wxID_EXIT, wxString::FromUTF8("Exit"));
    Append(file, wxString::FromUTF8("File"));

    // "Game" menu
    auto game = new wxMenu;
    game->Append(NEW_GAME, wxString::FromUTF8("New Game"));
    Append(game, wxString::FromUTF8("Game"));

    // "View" menu
    // Contains "Character" submenu
    auto view = new wxMenu;
    auto character = new wxMenu;
    wxUint32 radioItemPos = 0;
    for (auto & pair : availableCharactersState.GetAllCharacters()) {
        character->AppendRadioItem(CHARACTER_ID_LOWEST + radioItemPos, pair.first);
        // Character selection event
        Bind(wxEVT_MENU, [radioItemPos, &characterState](wxCommandEvent & event) {
            characterState.ChangeCharacterRadioPos(radioItemPos);
            characterState.NotifyAll();
        }, CHARACTER_ID_LOWEST + radioItemPos);
        ++radioItemPos;
    }
    characterRadioItemNum = radioItemPos;
    character->AppendSeparator();
    character->Append(ADD_CHARACTER, "Add New Character");
    character->Append(REMOVE_CHARACTER, "Remove Current Character");
    view->Append(CHARACTER_MENU, "Character", character);
    view->AppendSeparator();
    view->Append(VIEW_SETTINGS, "View Settings");
    Append(view, wxString::FromUTF8("View"));

    // "Help" menu
    auto help = new wxMenu;
    help->Append(wxID_ABOUT, wxString::FromUTF8("About"));
    Append(help, wxString::FromUTF8("Help"));

    /* Bind all menu clicked events */

    // Close
    Bind(wxEVT_MENU, [frame](wxCommandEvent & event){frame->Close();}, wxID_EXIT);

    // Open New Game Dialog
    Bind(wxEVT_MENU, [this, &gameState, frame](wxCommandEvent & event){
        // If a game has started, and hasn't ended yet, prompt player if restart is really ok
        if (promptUponNewGame) {
            wxMessageDialog askForRestartGame(frame,
                    "The game is still going on!\nWould you like to restart game?",
                    "Restart Game", wxOK | wxCANCEL | wxCANCEL_DEFAULT |
                    wxCENTER | wxICON_EXCLAMATION);
            if (askForRestartGame.ShowModal() != wxID_OK) return;
            gameState.ForceQuit();
            gameState.NotifyAll();
            promptUponNewGame = false;
        }
        NewGameDialog newGameDialog(frame);
        if (newGameDialog.ShowModal() == wxID_OK) { // If Start(OK) was pressed on the dialog.
            auto w = (unsigned) newGameDialog.GetRequestedWidth(),
                    h = (unsigned) newGameDialog.GetRequestedHeight();
            if (gameState.InitializeField(w, h, newGameDialog.GetRequestedNumOfMines(w, h)))
                gameState.NotifyAll();
            else
                wxMessageDialog (frame, "Unable to initialize the minefield.\n"
                                              "Please ensure that the field is not smaller than 9x9,\n"
                                              "and the ratio is set such that the number of mines does not exceed 999",
                                              "Minefield initialization unsuccessful", wxOK | wxOK_DEFAULT |
                                              wxCENTER | wxICON_EXCLAMATION).ShowModal();
        }
    }, NEW_GAME);

    // Open Character Editor dialog in add mode
    Bind(wxEVT_MENU, [frame, &states](wxCommandEvent & event){
        CharacterEditorDialog characterEditorDialog(frame, states, true);
        characterEditorDialog.ShowModal();
    }, ADD_CHARACTER);

    // Open Remove Character information dialog
    Bind(wxEVT_MENU, [&availableCharactersState, &characterState, character, frame, this](wxCommandEvent & event) {
        wxUint32 currentCharPos = characterState.GetCharacterRadioPos();
        auto characterNameToRemove = availableCharactersState.GetAllCharacters()[currentCharPos].first;
        wxMessageDialog dlg(frame, wxString::Format("Remove character \"%s\"? This cannot be undone!",
                availableCharactersState.GetAllCharacters()[currentCharPos].first),
                        "Remove Character?", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER | wxICON_QUESTION);
        if (dlg.ShowModal() == wxID_YES) {
            // Prevent the default character from being removed!
            if (!availableCharactersState.RemoveCharacter(currentCharPos)) {
                wxMessageDialog (frame, "Cannot remove the default character!", "Removal Unsuccessful",
                                 wxOK | wxOK_DEFAULT | wxCENTER | wxICON_EXCLAMATION).ShowModal();
                return;
            }
            character->Remove(CHARACTER_ID_LOWEST + currentCharPos);
            std::ifstream ifs("additional_chars.json");
            nlohmann::json j;
            ifs >> j;
            j.erase(j.begin() + currentCharPos);
            std::ofstream ofs("additional_chars.json");
            ofs << j;
            PRINT_MSG("File updated success");
            for (auto suffix : {"-normal", "-leftdown", "-win", "-lose"})
                wxRemoveFile(wxString::Format("img/%s%s.png", characterNameToRemove, suffix));
            --characterRadioItemNum;
            character->Check(CHARACTER_ID_LOWEST + (characterRadioItemNum - 1), true);
            characterState.ChangeCharacterRadioPos(characterRadioItemNum - 1);
            characterState.NotifyAll();
            wxASSERT_MSG(characterRadioItemNum == availableCharactersState.GetAllCharacters().size(),
                    wxString::Format("characterRadioItemNum: \"%d\" does not equal"
                                     "availableCharactersState.GetAllCharacters().size(): "
                                     " \"%d\".", characterRadioItemNum,
                                     availableCharactersState.GetAllCharacters().size()));
        }
    }, REMOVE_CHARACTER);

    // Open the About dialog.
    Bind(wxEVT_MENU, [frame](wxCommandEvent & event) {
        AboutDialog(frame).ShowModal();
    }, wxID_ABOUT);

    // Register. The function passed is what gets called upon notify all (Basically "update" function)
    // availableCharactersState registration is done at the end because it requires the Character submenu instantiated.
    availableCharactersState.Register([this, character, &characterState](const AvailableCharactersState & availableCharactersState) {
        auto & allCharacters = availableCharactersState.GetAllCharacters();
        // If a new character seems to have been added...
        if (allCharacters.size() > characterRadioItemNum) {
            wxUint32 insertPos = availableCharactersState.GetAffectedPosition();
            character->InsertRadioItem(insertPos, CHARACTER_ID_LOWEST + characterRadioItemNum,
                    allCharacters[insertPos].first);
            wxUint32 charRadItNum = characterRadioItemNum;
            Bind(wxEVT_MENU, [charRadItNum, &characterState](wxCommandEvent & event) {
                characterState.ChangeCharacterRadioPos(charRadItNum);
                characterState.NotifyAll();
            }, CHARACTER_ID_LOWEST + charRadItNum);
            ++characterRadioItemNum;
        } else {
            // update character info
            //character->GetMenuItems()[characterState.GetCharacterRadioPos()]->SetItemLabel(availableCharactersState.
            //    GetAllCharacters()[availableCharactersState.GetAffectedPosition()].first);
            // This feature will be implemented in a future version
        }
    });
}

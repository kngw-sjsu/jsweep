//
// The menubar that is used for a lot of purposes.
// Probably starting the game is all that really matters in this class..
// TO DO: Complete implementation of all menu items.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <iomanip>
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
    for (auto & eachCharacter : availableCharactersState.GetAllCharacters()) {
        PRINT_MSG("eachCharacter.windowId: " << eachCharacter.windowId);
        character->AppendRadioItem(eachCharacter.windowId, eachCharacter.name);
        // Character selection event
        Bind(wxEVT_MENU, [&characterState, &eachCharacter, radioItemPos](wxCommandEvent & event) {
            characterState.ChangeCharacter(eachCharacter);
            characterState.NotifyAll();
        }, eachCharacter.windowId);
        ++radioItemPos;
    }
    characterRadioItemNum = radioItemPos;
    // Check initial character as defined by config.json
    character->FindItem(characterState.GetCurrentCharacter().windowId)->Check();
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
    Bind(wxEVT_MENU, [this, &gameState, &states, frame](wxCommandEvent & event){
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
        NewGameDialog newGameDialog(frame, states);
        if (newGameDialog.ShowModal() == wxID_OK) { // If Start(OK) was pressed on the dialog.
            auto w = (unsigned) newGameDialog.GetRequestedWidth(),
                    h = (unsigned) newGameDialog.GetRequestedHeight();
            if (gameState.InitializeField(w, h, newGameDialog.GetRequestedRatio()))
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
        CharacterEditorDialog characterEditorDialog(frame, states);
        characterEditorDialog.ShowModal();
    }, ADD_CHARACTER);

    // Open Remove Character information dialog
    Bind(wxEVT_MENU, [&availableCharactersState, &characterState, character, frame, this](wxCommandEvent & event) {
        // wxUint32 radioItemPosToRemove = characterState.GetCharacterRadioPos();
        auto & characterToRemove = characterState.GetCurrentCharacter(); // availableCharactersState.GetAllCharacters()[radioItemPosToRemove];
        wxString nameOfRemovedCharacter = characterToRemove.name;
        wxInt32 wIdOfRemovedCharacter = characterToRemove.windowId;
        wxMessageDialog dlg(frame, wxString::Format("Remove character \"%s\"? This cannot be undone!",
            characterToRemove.name), "Remove Character?", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER | wxICON_QUESTION);
        if (dlg.ShowModal() == wxID_YES) {
            // Prevent the default character from being removed!
            if (!availableCharactersState.RemoveCharacter(characterToRemove)) {
                wxMessageDialog (frame, "Cannot remove the default character!", "Removal Unsuccessful",
                                 wxOK | wxOK_DEFAULT | wxCENTER | wxICON_EXCLAMATION).ShowModal();
                return;
            }
            character->Destroy(wIdOfRemovedCharacter);
            for (auto suffix : {"-normal", "-leftdown", "-win", "-lose"})
                wxRemoveFile(wxString::Format("img/%s%s.png", nameOfRemovedCharacter, suffix));
            PRINT_MSG("characterRadioItemNum: " << characterRadioItemNum);
            --characterRadioItemNum;
            PRINT_MSG("characterRadioItemNum: " << characterRadioItemNum);
            // wxInt32 newRadioPos = radioItemPosToRemove < characterRadioItemNum ?
            //    radioItemPosToRemove : radioItemPosToRemove - 1;
            auto & defaultCharacter = availableCharactersState.GetAllCharacters().front();
            character->FindItem(defaultCharacter.windowId)->Check();
            characterState.ChangeCharacter(defaultCharacter);
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
        // New characters will always be appended to the radio item so get the item at the end (size() - 1)
        auto & newCharacter = availableCharactersState.GetAllCharacters().back();
        character->InsertRadioItem(characterRadioItemNum, newCharacter.windowId, newCharacter.name);
        Bind(wxEVT_MENU, [&newCharacter, &characterState](wxCommandEvent &event) {
            characterState.ChangeCharacter(newCharacter);
            characterState.NotifyAll();
        }, newCharacter.windowId);
        ++characterRadioItemNum;
        // Update character feature has been cancelled.
    });
}

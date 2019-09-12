//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_MENUBAR_H
#define JSWEEP_MENUBAR_H

#include "common.h"

#include <wx/menu.h>
#include "state/AvailableCharactersState.h"
#include "state/CharacterState.h"
#include "state/GameState.h"

class MenuBar : public wxMenuBar {
    enum {
        NEW_GAME = 1000,
        CHARACTER_MENU,
        ADD_CHARACTER,
        REMOVE_CHARACTER,
        VIEW_SETTINGS,
        CHARACTER_ID_LOWEST = 2000
    };
    bool promptUponNewGame;
    wxUint32 characterRadioItemNum;
protected:

public:
    MenuBar(wxWindow* frame, States & states);
};


#endif //JSWEEP_MENUBAR_H

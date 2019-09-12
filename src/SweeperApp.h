//
// kngw-sjsu (c) 2019
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_SWEEPERAPP_H
#define JSWEEP_SWEEPERAPP_H

#include "common.h"

#include <wx/app.h>

#include "state/AvailableCharactersState.h"
#include "state/CharacterState.h"
#include "state/GameState.h"
#include "state/TimerState.h"

class SweeperApp : public wxApp {
    AvailableCharactersState availableCharactersState;
    CharacterState characterState;
    GameState gameState;
    TimerState timerState;
    States states{availableCharactersState, characterState, gameState, timerState};
public:
    bool OnInit() override;
};

#endif //JSWEEP_SWEEPERAPP_H

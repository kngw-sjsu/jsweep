//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_CHARACTERSTATE_H
#define JSWEEP_CHARACTERSTATE_H

#include <wx/bitmap.h>
#include "State.h"


class CharacterState : public State<CharacterState> {
public:
    /**
     * Enum used to decide which bitmap to use.
     * In a separate public access modifier since private member "status" below uses it.
     */
    enum State {
        NORMAL,
        DOWN,
        WIN,
        LOSE
    };
private:
    State status;
    wxUint32 characterRadioPos;
public:
    static const wxUint32 CHARACTER_IMAGE_LENGTH;

    CharacterState();
    ~CharacterState(){PRINT_MSG("CharacterState destr called");}

    /**
     * Updates character's radio item position to the one specified.
     * @param characterRadioPos The position of the radio item selected in the Character menu.
     */
    void ChangeCharacterRadioPos(wxUint32 characterRadioPos) {
        this->characterRadioPos = characterRadioPos;
    }

    /**
     * Changes character's status
     * @param status Enum of range 0-3 indicating character's facial expression.
     * Check the State enum for further details.
     */
    void ChangeStatus(State status) {
        this->status = status;
    }

    /**
     * Gets the position of the radio item currently selected.
     * @return The position of the radio item in the Character menu currently selected.
     */
    wxUint32 GetCharacterRadioPos() {
        return characterRadioPos;
    }

    /**
     * Gets the enum value denoting which facial expression.
     * @return The enum value denoting the current facial expression of the currently selected character.
     */
    State GetStatus() {
        return status;
    }
};


#endif //JSWEEP_CHARACTERSTATE_H

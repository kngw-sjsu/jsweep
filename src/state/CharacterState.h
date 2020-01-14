//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_CHARACTERSTATE_H
#define JSWEEP_CHARACTERSTATE_H

#include <wx/bitmap.h>
#include "Character.h"
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
    const Character* currentCharacter;
public:
    static const wxUint32 CHARACTER_IMAGE_LENGTH;

    CharacterState();
    ~CharacterState(){PRINT_MSG("CharacterState destr called");}

    /**
     * Updates character to the one specified.
     * @param newCharacter New character to display.
     */
    void ChangeCharacter(const Character & newCharacter) {
        currentCharacter = &newCharacter;
    }

    /**
     * Changes character's status
     * @param newStatus Enum of range 0-3 indicating character's facial expression.
     * Check the State enum for further details.
     */
    void ChangeStatus(State newStatus) {
        status = newStatus;
    }

    /**
     * Gets the currently displayed character.
     * @return Current character.
     */
    const Character & GetCurrentCharacter() {
        return *currentCharacter;
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

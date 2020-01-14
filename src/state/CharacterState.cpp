//
// Mainly listened to by MainPanel and Minefield.
// Holds information on which character, among the available ones in the Character menu,
//      are currently selected, and what facial expression he/she/it's supposed to make.
//
// Some misc information like the width and height of the bitmap is provided as well.
//

/*************************************/
/* Includes                          */
/*************************************/

#include "CharacterState.h"


/*************************************/
/* Definitions                       */
/*************************************/

// The character's image length.
const wxUint32 CharacterState::CHARACTER_IMAGE_LENGTH = 52;

CharacterState::CharacterState() :
    // Keeps track of the facial expression of the current character.
    status(State::NORMAL),

    // The pointer to our currently selected character.
    currentCharacter(nullptr) {}

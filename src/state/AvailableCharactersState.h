//
// Notifies MenuBar when a character has been added, info has been updated, or it has been removed.
// Also contains info about character names, as they are used in MenuBar for insertion of radio item
// ...when a new character has been added.
//

#ifndef JSWEEP_AVAILABLECHARACTERSSTATE_H
#define JSWEEP_AVAILABLECHARACTERSSTATE_H

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/bitmap.h>
#include "Character.h"
#include "State.h"


/*************************************/
/* Definitions                       */
/*************************************/
class AvailableCharactersState : public State<AvailableCharactersState> {
    // characters: Vector of character structs. Pretty much the same structure as the additional_chars
    //      array in config.json except it also contains the currently assigned Window ID plus the default character.
    std::list<Character> characters;
public:
    ~AvailableCharactersState(){PRINT_MSG("AvailableCharactersState destr called");}
    // When adding a new character, it is always prepended to the existing ones (last element), so push_back is Ok.
    /**
     * Adds a character to the private vector of available characters. Window ID is automatically assigned.
     * @param characterName The name of the character.
     * @param bitmaps A 4-element array (each representing facial expression as defined in Enum CharacterState::State)<br />
     *      of the bitmaps used to draw the character.
     */
    const Character & AddCharacter(const wxString & characterName, const std::array<wxBitmap, 4> & bitmaps) {
        // affectedPosition = characters.size();
        characters.emplace_back(characterName, wxWindow::NewControlId(), bitmaps);
        PRINT_MSG("characters.back().name: " << characters.back().name <<
            ", characters.back().windowId: " << characters.back().windowId <<
            ", &characters.back(): " << &characters.back());
        return characters.back();
    }

    /**
     * Gets the private vector of available characters.
     * @return The private vector of available characters.
     */
    const std::list<Character> & GetAllCharacters() const {
        return characters;
    }

    /**
     * Removes the character at the specified radio item position.
     * @param characterRadioPos The position of the current radio item corresponding to the character to remove from the menu.
     * @return True, if removal was successful.
     */
    bool RemoveCharacter(const Character & characterToRemove) {
        // Returns false, if attempted to remove the default character.
        if (characterToRemove == characters.front())
            return false;
        characters.remove(characterToRemove);
        return true;
    }
};


#endif //JSWEEP_AVAILABLECHARACTERSSTATE_H

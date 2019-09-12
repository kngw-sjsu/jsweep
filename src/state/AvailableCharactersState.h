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
#include "State.h"


/*************************************/
/* Definitions                       */
/*************************************/

class AvailableCharactersState : public State<AvailableCharactersState> {
    // More on the std::pair:
    //      wxBitmap: The underlying bitmap
    //      wxString: The path to the original image
    using CharacterVector = std::vector<std::pair<wxString, std::array<std::pair<wxBitmap, wxString>, 4>>>;

    // characters: Exact same structure as additional_chars.json except it also contains info on our default character.
    CharacterVector characters;

    // affectedPosition: Position among the radio items at which the new/updated character is. Read by MenuBar only.
    wxUint32 affectedPosition;
public:
    ~AvailableCharactersState(){PRINT_MSG("AvailableCharactersState destr called");}
    // When adding a new character, it is always prepended to the existing ones (last element), so push_back is Ok.
    /**
     * Adds a character to the private vector of available characters.
     * @param characterName The name of the character.
     * @param bitPathPair A 4-element array (each representing facial expression)
     *                  of the pair described above that holds all information needed for the character.
     */
    void AddCharacter(const wxString & characterName, const std::array<std::pair<wxBitmap, wxString>, 4> & bitPathPair) {
        affectedPosition = characters.size();
        characters.emplace_back(characterName, bitPathPair);
    }

    /**
     * Returns either the newly added or removed position of the radio item in the Character menu.
     * @return Position of the radio item to either add or remove.
     */
    wxUint32 GetAffectedPosition() const {
        return affectedPosition;
    }

    /**
     * Gets the private vector of available characters.
     * @return The private vector of available characters.
     */
    const CharacterVector & GetAllCharacters() const {
        return characters;
    }

    /**
     * Gets the bitmap of the current character corresponding to the status specified in the argument.
     * @param characterRadioPos The radio item position, in the Character menu, of the currently displayed character.
     * @param status Specifies which facial expression.
     * @return The corresponding bitmap.
     */
    const wxBitmap & GetCharacterBitmap(wxUint32 characterRadioPos, wxUint32 status) const {
        return characters[characterRadioPos].second[status].first;
    }

    /**
     * Removes the character at the specified radio item position.
     * @param characterRadioPos The position of the radio item in the Character menu to remove.
     * @return True, if removal was successful.
     */
    bool RemoveCharacter(wxUint32 characterRadioPos) {
        if (!characterRadioPos)
            return false;
        characters.erase(characters.begin() + characterRadioPos);
        return true;
    }
};


#endif //JSWEEP_AVAILABLECHARACTERSSTATE_H

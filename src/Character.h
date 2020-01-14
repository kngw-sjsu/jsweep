//
// A simple struct which holds information on a character available in the menu.
// This struct is to only be used by AvailableCharactersState and CharactersState classes.
//

#ifndef JSWEEP_CHARACTER_H
#define JSWEEP_CHARACTER_H

// name: The name of the character
// menuId: Corresponding ID assigned by wxWindow::NewControlId()
struct Character {
    wxString name;
    wxInt32 windowId;
    std::array<wxBitmap, 4> bitmaps;

    Character(const wxString & newName, wxInt32 newWindowId, const std::array<wxBitmap, 4> & newBitmaps) {
        name = newName;
        windowId = newWindowId;
        bitmaps = newBitmaps;
    }

    bool operator == (const Character & character) const {
        return name == character.name &&
            windowId == character.windowId &&
            bitmaps[0].IsSameAs(character.bitmaps[0]) &&
            bitmaps[1].IsSameAs(character.bitmaps[1]) &&
            bitmaps[2].IsSameAs(character.bitmaps[2]) &&
            bitmaps[3].IsSameAs(character.bitmaps[3]);
    }
};

#endif //JSWEEP_CHARACTER_H

//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_DIGITALFONTSET_H
#define JSWEEP_DIGITALFONTSET_H

#include <map>
#include <wx/image.h>

class DigitalFontSet {
    std::map<wxString, wxImage> textImages;
public:
    explicit DigitalFontSet(wxUint8 r = 0, wxUint8 g = 0, wxUint8 b = 0);

    /**
     * Gets the image of the text with the specified name.
     * @param name Name of the image stored in the strings private member variable.
     * @return The image with the given specified.
     */
    const wxImage & GetTextImage(const wxString & name) {
        return textImages[name];
    }

    void SetColor(wxUint8 r, wxUint8 g, wxUint8 b);
};


#endif //JSWEEP_DIGITALFONTSET_H

//
// This class contains letters/strings in digital-style font.
// For credit information on the author of the font used in this app, check /src/common.cpp
//

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/bitmap.h>
#include "DigitalFontSet.h"


/*************************************/
/* Definitions                       */
/*************************************/

DigitalFontSet::DigitalFontSet(wxUint8 r, wxUint8 g, wxUint8 b) :
        textImages({
            {"0", wxBITMAP_PNG(dig0).ConvertToImage()},
            {"1", wxBITMAP_PNG(dig1).ConvertToImage()},
            {"2", wxBITMAP_PNG(dig2).ConvertToImage()},
            {"3", wxBITMAP_PNG(dig3).ConvertToImage()},
            {"4", wxBITMAP_PNG(dig4).ConvertToImage()},
            {"5", wxBITMAP_PNG(dig5).ConvertToImage()},
            {"6", wxBITMAP_PNG(dig6).ConvertToImage()},
            {"7", wxBITMAP_PNG(dig7).ConvertToImage()},
            {"8", wxBITMAP_PNG(dig8).ConvertToImage()},
            {"9", wxBITMAP_PNG(dig9).ConvertToImage()},
            {"neg", wxBITMAP_PNG(digneg).ConvertToImage()},
            {"perc", wxBITMAP_PNG(digperc).ConvertToImage()},
        }) {
    for (auto & string : textImages) {
        auto & theImage = string.second;
        theImage.SetRGB(wxRect(0, 0, theImage.GetWidth(), theImage.GetHeight()), r, g, b);
    }
}

/**
 * Sets the color of the string.
 * @param r Red.
 * @param g Green.
 * @param b Blue.
 */
void DigitalFontSet::SetColor(wxUint8 r, wxUint8 g, wxUint8 b) {
    for (auto & string : textImages) {
        auto & theImage = string.second;
        theImage.SetRGB(wxRect(0, 0, theImage.GetWidth(), theImage.GetHeight()), r, g, b);
    }
}

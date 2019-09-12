//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_MINEFIELD_H
#define JSWEEP_MINEFIELD_H

#include "common.h"

class Minefield : public wxWindow {
    static const wxUint32
        GRID_PIXEL_SIZE,
        GRID_BORDER_WIDTH;

    class GridNumberBitmapArray {
        std::array<wxBitmap, 8> numberBitmaps;
    public:
        GridNumberBitmapArray();
        const wxBitmap & operator[] (wxUint32 pos) const {
            return numberBitmaps[pos];
        }
    };
    wxBitmap mine;
    wxPoint leftCurrentlyDownPos;
    wxSize dimensions; // in pixels
protected:
    wxSize DoGetBestClientSize() const override;
public:
    Minefield(wxWindow* parent, States & states);
};


#endif //JSWEEP_MINEFIELD_H

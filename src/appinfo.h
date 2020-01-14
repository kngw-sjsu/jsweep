//
// Contains various app-related information such as version info.
//

#ifndef JSWEEP_APPINFO_H
#define JSWEEP_APPINFO_H

#include <wx/string.h>

namespace AppInfo {

    /**
     * @return Author of this app. Me.
     */
    wxString GetAuthor() {
        return "kngw-sjsu";
    };

     /**
      * @return Version number.
      */
     wxString GetVersion() {
        return "0.1.0";
    }

    /**
     * @return List of libraries/resources/software used for the development of the app.
     */
    std::array<wxString, 4> GetUsedResources() {
        return {
            "wxWidgets",
            "JSON for Modern C++ by nlohmann",
            "Letters Laughing at their Execution by Glyphobet Font Foundry",
            "Vegur by Sora Sagano (dotcolon)"
        };
    }
}

#endif //JSWEEP_APPINFO_H

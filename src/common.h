//
// Contains includes of header files that are barely ever or never modified,
//      as well as some typedefs and usings that are used quite frequently throughout the whole src tree.
//

#ifndef JSWEEP_COMMON_H
#define JSWEEP_COMMON_H

// Includes of headers that are done in virtually every class.
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
#include <tuple>
#include <vector>
#include <list>
#include <wx/debug.h>
#include <wx/sizer.h>
#include <wx/panel.h>

// Used to print debug-related messages
#ifdef __WXDEBUG__
#   define PRINT_MSG(msg) std::cout << msg << std::endl
#else
#   define PRINT_MSG(msg)
#endif


// Misc global constants, typedefs and usings used throughout the sources.

class AvailableCharactersState;
class CharacterState;
class GameState;
class TimerState;
using States = std::tuple<AvailableCharactersState &, CharacterState &, GameState &, TimerState &>;

#endif //JSWEEP_COMMON_H

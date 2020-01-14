//
// The main panel. Contains the minefield where the player clicks on,
// ...the timer, the small window showing how many mines are left,
// ...and the lovely characters to be displayed in the middle.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include "MainPanel.h"
#include "Minefield.h"

#include "state/AvailableCharactersState.h"
#include "state/CharacterState.h"
#include "state/GameState.h"
#include "state/TimerState.h"


/*************************************/
/* Definitions                       */
/*************************************/

const wxInt32 MainPanel::NUMBER_WINDOW_WIDTH = 58;
const wxInt32 MainPanel::NUMBER_WINDOW_HEIGHT = 32;

MainPanel::MainPanel(wxWindow* parent, States & states) : wxPanel(parent), timeElapsed(-1),
        mineNumFontSet(65, 0, 0), timerFontSet(0, 65, 0) {
    // 'Extract' the states that will be used in this class
    auto & availableCharactersState = std::get<AvailableCharactersState &>(states);
    auto & characterState = std::get<CharacterState &>(states);
    auto & gameState = std::get<GameState &>(states);
    auto & timerState = std::get<TimerState &>(states);

    auto panelSizer = new wxBoxSizer(wxVERTICAL);

    // Sizer that contains the character, # of mines, and the timer.
    auto statusSizer = new wxBoxSizer(wxHORIZONTAL);
    auto mineNumView = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxSize(NUMBER_WINDOW_WIDTH,NUMBER_WINDOW_HEIGHT));
    mineNumView->SetBackgroundStyle(wxBG_STYLE_PAINT);
    mineNumView->Bind(wxEVT_PAINT, [this, mineNumView, &gameState](wxPaintEvent & event) {
        wxAutoBufferedPaintDC dc(mineNumView);
        if (auto context = wxGraphicsContext::Create(dc)) {
            context->SetBrush(wxBrush(wxColour(33, 33, 33)));
            context->DrawRectangle(0,0,mineNumView->GetSize().x,mineNumView->GetSize().y);
            wxInt32 numOfMines = gameState.GetNumOfMines();
            if (numOfMines < 0) {
                auto & neg = mineNumFontSet.GetTextImage("neg");
                context->DrawBitmap(wxBitmap(neg), (NUMBER_WINDOW_WIDTH/2.0)-(neg.GetWidth()/2.0),
                    (NUMBER_WINDOW_HEIGHT/2.0)-(neg.GetHeight()/2.0), neg.GetWidth(), neg.GetHeight());
            } else {
                auto
                    & d1 = mineNumFontSet.GetTextImage(gameState.IsFieldGenerated() ?
                                                       wxString::Format("%d", numOfMines % 10) : "8"),
                    & d10 = mineNumFontSet.GetTextImage(gameState.IsFieldGenerated() ?
                                                        wxString::Format("%d", (numOfMines % 100) / 10) : "8"),
                    & d100 = mineNumFontSet.GetTextImage(gameState.IsFieldGenerated() ?
                                                         wxString::Format("%d", numOfMines / 100) : "8");
                int startingXPoint = (NUMBER_WINDOW_WIDTH / 2) - ((d1.GetWidth() + d10.GetWidth() + d100.GetWidth()) / 2);
                context->DrawBitmap(wxBitmap(d100), startingXPoint,
                    (NUMBER_WINDOW_HEIGHT/2.0)-(d100.GetHeight()/2.0), d100.GetWidth(), d100.GetHeight());
                context->DrawBitmap(wxBitmap(d10), startingXPoint + d100.GetWidth(),
                    (NUMBER_WINDOW_HEIGHT/2.0)-(d10.GetHeight()/2.0), d10.GetWidth(), d10.GetHeight());
                context->DrawBitmap(wxBitmap(d1), startingXPoint + d100.GetWidth() + d10.GetWidth(),
                    (NUMBER_WINDOW_HEIGHT/2.0)-(d1.GetHeight()/2.0), d1.GetWidth(), d1.GetHeight());
            }
            delete context;
        }
    });
    statusSizer->Add(mineNumView, 0, wxALIGN_CENTER_VERTICAL);
    statusSizer->AddStretchSpacer(1);

    auto characterView = new wxWindow(this, wxID_ANY, wxDefaultPosition,
            wxSize(CharacterState::CHARACTER_IMAGE_LENGTH, CharacterState::CHARACTER_IMAGE_LENGTH));
    characterView->SetBackgroundStyle(wxBG_STYLE_PAINT);
    characterView->Bind(wxEVT_PAINT, [this, &characterState, characterView](wxPaintEvent & event) {
        wxAutoBufferedPaintDC dc(characterView);
        dc.Clear();
        if (auto context = wxGraphicsContext::Create(dc)) {
            context->DrawBitmap(characterState.GetCurrentCharacter().bitmaps[characterState.GetStatus()], 0, 0,
                                CharacterState::CHARACTER_IMAGE_LENGTH, CharacterState::CHARACTER_IMAGE_LENGTH);
            delete context;
        }
    });
    // characterView = new CharacterState(this);

    // Also add mouse click event for character reaction
    statusSizer->Add(characterView);
    statusSizer->AddStretchSpacer(1);
    auto timerViewSize = wxSize(NUMBER_WINDOW_WIDTH,NUMBER_WINDOW_HEIGHT);
    auto timerView = new wxWindow(this, wxID_ANY, wxDefaultPosition, timerViewSize);
    timerView->SetBackgroundStyle(wxBG_STYLE_PAINT);
    timerView->Bind(wxEVT_PAINT, [this, timerViewSize, timerView](wxPaintEvent & event) {
        wxAutoBufferedPaintDC dc(timerView);
        if (auto context = wxGraphicsContext::Create(dc)) {
            context->SetBrush(wxBrush(wxColour(33, 33, 33)));
            context->DrawRectangle(0, 0, timerViewSize.x, timerViewSize.y);
            auto
                & d1M = timerFontSet.GetTextImage(
                    timeElapsed >= 0 ? wxString::Format("%d", (timeElapsed / 60) % 10) : "8"),
                & d10M = timerFontSet.GetTextImage(timeElapsed >= 0 ? wxString::Format("%d", timeElapsed / 600) : "8"),
                & d1S = timerFontSet.GetTextImage(
                    timeElapsed >= 0 ? wxString::Format("%d", (timeElapsed % 60) % 10) : "8"),
                & d10S = timerFontSet.GetTextImage(
                    timeElapsed >= 0 ? wxString::Format("%d", (timeElapsed % 60) / 10) : "8");
            context->DrawBitmap(wxBitmap(d10M), (NUMBER_WINDOW_WIDTH/4.0) - ((d1M.GetWidth() + d10M.GetWidth())/2.0),
                (NUMBER_WINDOW_HEIGHT/2.0) - (d10M.GetHeight()/2.0), d10M.GetWidth(), d10M.GetHeight());
            context->DrawBitmap(wxBitmap(d1M), (NUMBER_WINDOW_WIDTH/4.0) - ((d1M.GetWidth() + d10M.GetWidth())/2.0) + d10M.GetWidth(),
                (NUMBER_WINDOW_HEIGHT/2.0) - (d1M.GetHeight()/2.0), d1M.GetWidth(), d1M.GetHeight());
            context->DrawBitmap(wxBitmap(d10S), ((3*NUMBER_WINDOW_WIDTH)/4.0) - ((d1S.GetWidth() + d10S.GetWidth())/2.0),
                (NUMBER_WINDOW_HEIGHT/2.0) - (d10S.GetHeight()/2.0), d10S.GetWidth(), d10S.GetHeight());
            context->DrawBitmap(wxBitmap(d1S), ((3*NUMBER_WINDOW_WIDTH)/4.0) - ((d1S.GetWidth() + d10S.GetWidth())/2.0) + d10S.GetWidth(),
                (NUMBER_WINDOW_HEIGHT/2.0) - (d1S.GetHeight()/2.0), d1S.GetWidth(), d1S.GetHeight());
            delete context;
        }
    });
    timerView->Bind(wxEVT_TIMER, [this, &timerState, timerView](wxTimerEvent & event) {
        if (++timeElapsed > 5999) // count stop if 99:59
            timerState.Stop(timeElapsed);
        timerView->Refresh();
    });
    timerState.SetWindowToBeTimed(timerView); // Bind timerView to the underlying timer
    statusSizer->Add(timerView, 0, wxALIGN_CENTER_VERTICAL);
    panelSizer->Add(statusSizer, 0, wxEXPAND);

    // Can't begin a game without this!
    auto minefield = new Minefield(this, states);
    minefield->Bind(wxEVT_SIZE, [this](wxSizeEvent & event) {
        PRINT_MSG("EVT_SIZE triggered\nw: " << GetSize().x << ", h: " << GetSize().y);
        GetParent()->Fit();
    });

    panelSizer->Add(minefield);

    SetSizer(panelSizer); // Do not use SetSizeAndFit or else the frame will not get smaller if
    // ...any map smaller than (w:12,h:9) is generated.
    Fit();

    // Register. The function passed is what gets called upon notify all (Basically "update" function)
    // Adding this at the very end because need access to some of MainPanel's components
    characterState.Register([characterView](const CharacterState & characterState) {
        characterView->Refresh();
    });

    gameState.Register([this, &timerState, mineNumView, timerView](const GameState & gameState) {
        // Update count
        mineNumView->Refresh();
        // If field generated but game not started yet, make the time 0.
        if (gameState.IsReadyToPlay() && !gameState.IsInGame()) {
            timeElapsed = 0;
            timerView->Refresh();
            // Also update the font's color for mineNumView and timerView
            mineNumFontSet.SetColor(255, 0, 0);
            timerFontSet.SetColor(0, 255, 0);
        } else {
            // start or stop timer
            // If in game but timer is not, start the timer
            if (gameState.IsInGame() && !timerState.IsRunning())
                timerState.Start();
            // If not in game but timer still going on, stop it. The game is over.
            else if (!gameState.IsInGame() && timerState.IsRunning())
                timerState.Stop(timeElapsed);
        }
    });
}

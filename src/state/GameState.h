//
// Please check the corresponding CPP file for more info.
//

#ifndef JSWEEP_GAMESTATE_H
#define JSWEEP_GAMESTATE_H

#include <memory>
#include <random>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include "State.h"

class GameState : public State<GameState> {
    static const std::array<wxInt32, 2> FLAG_STATE_2_NMINES_UPDATE;

    /**
     * Grid object that holds information on:
     *      x: The horizontal position from the top-left origin.
     *      y: The vertical position from the top-left origin.
     *      nSurroundingMines: # of mines that surround the grid.
     *      isMineGrid: Flag that gets set to true, if the grid contains a mine.
     *      isRevealed: Flag that gets set to true, if the user left-clicks (reveals) the grid.
     *      isFlagged: Flag that gets set to true, if the user right-clicks (flags) the grid.
     */
    struct Grid {
        wxInt32 x, y, nSurroundingMines;
        bool isMineGrid, isRevealed, isFlagged;
    };

    bool fieldGenerated, readyToPlay, inGame, playerWin;
    wxUint32 width, height, numOfMines, numOfNonRevealedGrids;

    // List of grids that contain mines to keep track of which are remaining and what not.
    std::list<Grid*> bombGrids;

    // 2D array representation of the minefield.
    std::vector<std::vector<Grid>> grids;

    void PlaceMines(wxInt32 numOfMines, const wxRect & area, std::mt19937 & engine);
    static void SelectMinePositionsToRemove(wxInt32 remaining, wxInt32 startPos, wxInt32 endPos,
            std::list<wxInt32> & positions, std::mt19937 & engine);
public:
    GameState();
    ~GameState(){PRINT_MSG("GameState destr called");}

    /**
     * Flag or unflag the right-clicked grid.
     * @param x The x position of the right-clicked grid.
     * @param y The y position of the right-clicked grid.
     */
    void ChangeFlagState(wxInt32 x, wxInt32 y) {
        grids[y][x].isFlagged ^= 1;
        numOfMines += FLAG_STATE_2_NMINES_UPDATE[grids[y][x].isFlagged];
    }

    /**
     * Ends the game despite not having met all the conditions to win/lose the game.
     */
    void ForceQuit() {
        inGame = false;
        readyToPlay = false;
    }

    /**
     * Gets the underlying 2D array representation of the minefield.
     * @return 2D array representation of the minefield.
     */
    const std::vector<std::vector<Grid>> & GetGridMap() const {
        return grids;
    }

    /**
     * Gets the size of the minefield.
     * @return wxSize of the minefield.
     */
    wxSize GetSize() const {
        return wxSize(width, height);
    }

    /**
     * Gets the number of mines remaining to be flagged.
     * @return The number of remaining mines.
     */
    wxUint32 GetNumOfMines() const {
        return numOfMines;
    }

    /**
     * Returns the flag detnoting if the minefield has been generated. Used for default screen checking.
     * @return Flag denoting whether or not the minefield has been generated.
     */
    bool IsFieldGenerated() const {
        return fieldGenerated;
    }

    /**
     * Returns the flag denoting if the game can be begun by left-clicking anywhere on the minefield.
     * @return Flag denoting whether or not the game is ready to start.
     */
    bool IsReadyToPlay() const {
        return readyToPlay;
    }

    /**
     * Returns the flag denoting if the game is still going on.
     * @return Flag denoting whether or not the game is still going on.
     */
    bool IsInGame() const {
        return inGame;
    }

    /**
     * Returns the flag denoting if the player has met all the conditions to win the round.
     * @return Flag denoting whether or not the player has just won.
     */
    bool PlayerWins() const {
        return playerWin;
    }

    bool AttemptToEndGame();
    bool BeginGame(wxInt32 clickedX, wxInt32 clickedY);
    bool InitializeField(wxUint32 width, wxUint32 height, wxUint32 numOfMines);
    void RevealGrid(wxInt32 clickedX, wxInt32 clickedY);
};


#endif //JSWEEP_GAMESTATE_H

//
// Notifies observers RootFrame, MainPanel and Minefield classes as the subject.
// Contains the game logic. Keep tracks of whatever user does on the app during a game.
// TO DO: Create EndGame function (might decide not to)
// TO DO 2: Rewrite some of the member function description.
// TO DO 3: Make the "SelectMinePositionsToRemove" function an unnamed namespace function.
// TO DO 4: Come up with better contents for the do-while loop in "RevealGrid" member function.
//

/*************************************/
/* Includes                          */
/*************************************/

#include <array>
#include <ctime>
#include <stack>
#include "GameState.h"


/*************************************/
/* Definitions                       */
/*************************************/

// Used internally to increment or decrement the number of mines by flagging,
//      since flagging can be toggled, meaning the number of mines can increase or decrease at player's will.
const std::array<wxInt32, 2> GameState::FLAG_STATE_2_NMINES_UPDATE = {1, -1};

GameState::GameState() :
    // Flag to check if default screen is currently shown.
    fieldGenerated(false),

    // Flag to check whether or not a game is currently going on
    inGame(false),

    // Flag to check if the minefield is initialized, and a game is ready to begin with user clicking it.
    readyToPlay(false),

    // Flag to check if the player has won.
    playerWin(false),

    // The # of grids in the horizontal direction.
    width(0),

    // The # of grids in the vertical direction.
    height(0),

    // What the variable name says.
    numOfMines(0),

    // The # of non-mine grids that have yet to be revealed
    numOfNonRevealedGrids(0) {}

/**
 * Attempts to end the game（適当）
 * @return True, if the game was successfully ended. false, if the game still needs to go on.
 */
bool GameState::AttemptToEndGame() {
    if (!inGame) { // If player loses, inGame flag is already false
        return true;
    } else if (!numOfMines && !numOfNonRevealedGrids) {
        // Player must have already clicked all non-mine grids &&
        // ...all mine grids must have already been flagged IN ORDER TO win.
        inGame = false;
        playerWin = true;
        readyToPlay = false;
        PRINT_MSG("Lemon squeezy\nnumOfNonRevealedGrids: " << numOfNonRevealedGrids);
        return true;
    }
    return false;
}

/**
 * Initializes the minefield by allocating mines to it and stuff IF the field is generated.
 * Called when the user right-clicks the minefield AND WHEN the field is generated.
 * @param width The # of grids in the horizontal direction.
 * @param height The # of grids in the vertical direction.
 * @param numOfMines The # of mines to be allocated across the minefield.
 * @return True, if minefield data initialization was successful.
 */
bool GameState::InitializeField(wxUint32 width, wxUint32 height, wxUint32 numOfMines) {
    PRINT_MSG("width: " << width << "\nheight: "<< height << "\nnumOfMines: " << numOfMines);

    // Cannot create map if...
    //      The map was specified to be of size smaller than 9x9.
    //      The number of mines to place is greater than 999.
    if (width < 9 || height < 9 || numOfMines > 999)
        return false;

    this->width = width;
    this->height = height;
    this->numOfMines = numOfMines;

    // To be SURE that conditions for starting a game have been met
    readyToPlay = width >= 0 && height >= 0 && numOfMines >= 0;

    // If previous game exists, clear the field first
    if (!grids.empty()) grids.clear();

    // Initialize array representation of the field
    grids.reserve(height);
    for (wxInt32 y = 0; y < height; ++y) {
        std::vector<Grid> eachRow;
        eachRow.reserve(width);
        for (wxInt32 x = 0; x < width; ++x)
            eachRow.push_back(Grid{x, y});
        grids.push_back(eachRow);
    }
    fieldGenerated = true;
    return true;
}

/**
 * Starts a new game. Field must already be generated and the game must not be going on.
 * @param clickedX The horizontal position of the clicked grid.
 * @param clickedY The vertical position of the clicked grid.
 * @return True, if game initialization was successful. False, if otherwise.
 */
bool GameState::BeginGame(wxInt32 clickedX, wxInt32 clickedY) {
    if (!readyToPlay || inGame)
        return false;

    // If previous game exists, clear all of the list
    if (!bombGrids.empty()) bombGrids.clear();

    // If player won the previous game, reset it to false
    playerWin = false;
    inGame = true;

    std::mt19937 engine((wxUint32)std::time(nullptr));

    // Reason for placing 9 extra mines is that,
    //      there could be likelihood of mines being placed in the forbidden area
    //      (The 3x3 area where the center grid is the one the user left-clicked),
    //      in which mines should never be placed.
    //      If there are still any excess mines left after removal from the forbidden area,
    //      remove mines randomly from the minefield using the SelectMinePositionsToRemove helper function.
    PlaceMines(numOfMines + 9, wxRect(0, 0, width, height), engine);

    // Get rid of all bombs in the forbidden area
    for (wxInt32 y = clickedY - 1; y < clickedY + 2; ++y) {
        if (y < 0 || y >= height) continue;
        for (wxInt32 x = clickedX - 1; x < clickedX + 2; ++x) {
            if (x < 0 || x >= width) continue;
            grids[y][x].isMineGrid = false;
        }
    }
    bombGrids.remove_if([](Grid* grid){return !grid->isMineGrid;});
    if (wxInt32 remaining = bombGrids.size() - numOfMines) {
        PRINT_MSG("bombGrids.size() after first removal: " << bombGrids.size());
        PRINT_MSG("Remaining after first removal: " << remaining);
        std::list<wxInt32> positions;
        SelectMinePositionsToRemove(remaining, 0, bombGrids.size() - 1, positions, engine);
        auto itr = positions.begin();
        wxInt32 n = -1;
        // positions are automatically sorted in ascending order
        bombGrids.remove_if([&itr, &n, this](Grid* grid){
            if (++n == *itr) {
                grid->isMineGrid = false;
                ++itr;
                return true;
            }
            return false;
        });
        PRINT_MSG("bombGrids.size() after second removal: " << bombGrids.size());
        wxASSERT_MSG(bombGrids.size() == numOfMines, "The # of mine grids stored internally does not "
                                                     "match the # of mines requested for field initialization.");

    }

    // Set number of surrounding mines for all relevant grids.
    for (auto grid : bombGrids) {
        for (wxInt32 y = grid->y - 1; y < grid->y + 2; y++) {
            if (y < 0 || y >= height) continue;
            for (wxInt32 x = grid->x - 1; x < grid->x + 2; x++) {
                if (x < 0 || x >= width || (grid->x == x && grid->y == y)) continue; // skip self as well
                ++grids[y][x].nSurroundingMines;
            }
        }
    }

    // Finally set the number of grids that don't contain mines
    numOfNonRevealedGrids = (width * height) - numOfMines;

    return true;
}

/**
 * PlaceMines: Private helper method that recursively places mines in the field.
 * @param numOfMines The number of mines to place across the minefield.
 * @param area The specified sub-rectangle for recursive mine placement.
 * @param engine The good old mt19937 random number generator engine (??) we all love and adore.
 */
// Memo: Horizontal: %, Vertical: /
void GameState::PlaceMines(wxInt32 numOfMines, const wxRect & area, std::mt19937 & engine) {
    if (!numOfMines) return;
    if (numOfMines == 1) {
        wxInt32
            size = area.width * area.height,
            position = std::uniform_int_distribution<>(0, size - 1)(engine),
            bombX = area.x + (position % area.width),
                bombY = area.y + (position / area.width);
        grids[bombY][bombX].isMineGrid = true;
        bombGrids.push_back(&grids[bombY][bombX]);
        return;
    }

    // Must check the size of each region.
    // Values are stored in increasing order to the nMinesInEachGrid array.
    std::array<wxInt32, 4> nMinesInEachGrid;
    nMinesInEachGrid[0] = numOfMines / 4;
    wxInt32 rest = numOfMines - nMinesInEachGrid[0];
    nMinesInEachGrid[1] = rest / 3;
    rest -= nMinesInEachGrid[1];
    nMinesInEachGrid[2] = rest / 2;
    nMinesInEachGrid[3] = rest - nMinesInEachGrid[2];

    // If all sub-regions allocated same of mines OR width and height of the parent region are both even.
    if (!(numOfMines % 4) || (!(area.width % 2) && !(area.height % 2))) {
        std::shuffle(nMinesInEachGrid.begin(), nMinesInEachGrid.end(), engine);
    } else if (area.width % 2 && !(area.height % 2)) { // If parent width odd and height even
        std::array<wxInt32, 2> smaller = {nMinesInEachGrid[0], nMinesInEachGrid[1]};
        std::array<wxInt32, 2> larger = {nMinesInEachGrid[2], nMinesInEachGrid[3]};
        std::shuffle(smaller.begin(), smaller.end(), engine);
        std::shuffle(larger.begin(), larger.end(), engine);
        nMinesInEachGrid[0] = larger[0];
        nMinesInEachGrid[1] = smaller[0];
        nMinesInEachGrid[2] = larger[1];
        nMinesInEachGrid[3] = smaller[1];
    } else if (!(area.width % 2) && area.height % 2) { // If parent width even and height odd
        std::shuffle(nMinesInEachGrid.begin(), nMinesInEachGrid.begin() + 2, engine); // smaller
        std::shuffle(nMinesInEachGrid.begin() + 2, nMinesInEachGrid.end(), engine); // larger
        wxInt32 smallerTemp1 = nMinesInEachGrid[0],
            smallerTemp2 = nMinesInEachGrid[1];
        nMinesInEachGrid[0] = nMinesInEachGrid[2];
        nMinesInEachGrid[1] = nMinesInEachGrid[3];
        nMinesInEachGrid[2] = smallerTemp1;
        nMinesInEachGrid[3] = smallerTemp2;
    } else { // q1 > q2 == q3 > q4
        // Switch q1 and q4
        wxInt32 smallest = nMinesInEachGrid[0];
        nMinesInEachGrid[0] = nMinesInEachGrid[3];
        nMinesInEachGrid[3] = smallest;

        // check if each area's size is 0 (i.e. q2 > q3 or q3 > q2)
        if (area.width > 1 && area.height > 1) { // if both q2 and q3 are existent
            if (area.width == area.height) // shuffle if both have the same size
                std::shuffle(nMinesInEachGrid.begin() + 1, nMinesInEachGrid.end() - 1, engine);
            else if (area.width > area.height) { // if q2 > q3
                wxInt32 temp = nMinesInEachGrid[1];
                nMinesInEachGrid[1] = nMinesInEachGrid[2];
                nMinesInEachGrid[2] = temp;
            }
            // if q3 > q2, do nothing because nMinesInEachGrid is already in ascending order
        } else if (area.width > 1 && area.height == 1) { // q3 non-existent, put in q2
            nMinesInEachGrid[1] = nMinesInEachGrid[2]; // put what is in q3 in q2
            nMinesInEachGrid[2] = 0; // since q3 is non-existent, just put zero in it
        }
        // if q2 is non-existent, do nothing because nMinesInEachGrid is already in ascending order
    }
    PRINT_MSG("w: " << area.width << ", h: " << area.height << ", n1: " << nMinesInEachGrid[0] <<
        ", n2: " << nMinesInEachGrid[1] << ", n3: " << nMinesInEachGrid[2] << ", n4: " << nMinesInEachGrid[3]);

    // Checking for zero here to avoid calling of functions which take longer than branch statements
    if (nMinesInEachGrid[0])
        PlaceMines(nMinesInEachGrid[0], wxRect(area.x, area.y,
                area.width - (area.width / 2), area.height - (area.height / 2)), engine); // square 1
    if (nMinesInEachGrid[1])
        PlaceMines(nMinesInEachGrid[1], wxRect(area.x + (area.width - (area.width / 2)), area.y,
                area.width / 2, area.height - (area.height / 2)), engine); // square 2
    if (nMinesInEachGrid[2])
        PlaceMines(nMinesInEachGrid[2], wxRect(area.x, area.y + (area.height - (area.height / 2)),
                area.width - (area.width / 2), area.height / 2), engine); // square 3
    if (nMinesInEachGrid[3])
        PlaceMines(nMinesInEachGrid[3], wxRect(area.x + (area.width - (area.width / 2)),
                area.y + (area.height - (area.height / 2)), area.width / 2, area.height / 2), engine); // square 4

}

/**
 * Private helper method that recursively selects positions from bombGrids to remove.
 * Called right after placing mines in the BeginGame member function.
 * @param remaining The number of 'excess' mines to remove.
 * @param startPos Starting position of the "positions" list.
 * @param endPos Last position of the "positions" list.
 * @param positions The list of the integer representation of the position of each mine grid.
 *                  "remaining" # of mines will be removed from this.
 * @param engine Random number generator.
 */
void GameState::SelectMinePositionsToRemove(wxInt32 remaining, wxInt32 startPos, wxInt32 endPos,
        std::list<wxInt32> & positions, std::mt19937 & engine) {
    // If numbers to put finally = 1, put it in a random position
    if (remaining == 1) {
        positions.push_back(std::uniform_int_distribution<>(startPos, endPos)(engine));
        return;
    }

    wxInt32 leftRemaining = remaining / 2,
        rightRemaining = leftRemaining;

    // If odd, randomly choose which side to put the larger one on
    if (remaining % 2) {
        // true: right, false: left
        if (std::bernoulli_distribution()(engine))
            ++rightRemaining;
        else
            ++leftRemaining;
    }

    // if length of current section is odd, always right gets the longer
    wxInt32 p = startPos + (((endPos + 1) - startPos) / 2);
    SelectMinePositionsToRemove(leftRemaining, startPos, p - 1, positions, engine);
    SelectMinePositionsToRemove(rightRemaining, p, endPos, positions, engine);
}

/**
 * Reveals the grid left-clicked by the player.
 * If the clicked grid does not have any surrounding mines, keep
 * @param clickedX The horizontal position of the left-clicked grid.
 * @param clickedY The vertical position of the left-clicked grid.
 */
void GameState::RevealGrid(wxInt32 clickedX, wxInt32 clickedY) {
    if (grids[clickedY][clickedX].isMineGrid) {
        // no need to update playerWin because it is already false
        inGame = false;
        readyToPlay = false;
        PRINT_MSG("Boom! Game over!");
        return;
    } // game over
    // typedef std::tuple<Grid*, int, int> GridAndPosition;
    // std::stack<GridAndPosition> stack;
    std::stack<Grid*> stack;
    // stack.emplace(&grids[clickedY][clickedX], clickedX - 1, clickedY - 1); // before
    auto centerGrid = &grids[clickedY][clickedX];
    centerGrid->isRevealed = true;
    --numOfNonRevealedGrids;
    if (!centerGrid->nSurroundingMines) {
        stack.emplace(centerGrid); // stack.emplace(centerGrid, clickedX - 1, clickedY - 1);
        do {
            auto grid = stack.top(); // std::get<0>(stack.top());
            // int *cx = &std::get<1>(stack.top()),
            // *cy = &std::get<2>(stack.top());
            // std::cout << "Grid x: " << grid->x << ", y: " << grid->y << ", *cx: " << *cx <<
            //     ", *cy: " << *cy << std::endl;
            for (wxInt32 y = grid->y - 1; y < grid->y + 2; ++y) {
                if (y < 0 || y >= height) continue;
                // grid->isRevealed = true;
                for (wxInt32 x = grid->x - 1; x < grid->x + 2; ++x) {
                    //std::cout << "Scanning x: " << x << ", y: " << y <<
                    //    ", isRevealed: " << grids[y][x].isRevealed << std::endl;
                    if (grids[y][x].isRevealed || grids[y][x].isFlagged || grids[y][x].isMineGrid || x < 0 || x >= width) continue;
                    grids[y][x].isRevealed = true; // mark this grid 'accessed'
                    --numOfNonRevealedGrids;
                    if (!grids[y][x].nSurroundingMines) { // If no surrounding mines, move to this grid
                        //*cx = x; // Update positions in the tuple so program knows
                        //*cy = y; // ...which of the surrounding grids to continue scanning
                        stack.emplace(&grids[y][x]); // stack.emplace(&grids[y][x], x - 1, y - 1);
                        stack.emplace(nullptr); // stack.emplace(nullptr, -1, -1); // dummy, will be popped out soon
                        y = 10000; // to force break out of the outer for loop
                        break;
                    }
                }
            }
            stack.pop();
        } while (!stack.empty());
    }
    // std::cout << "numOfNonRevealedGrids: " << numOfNonRevealedGrids << std::endl; // d
}

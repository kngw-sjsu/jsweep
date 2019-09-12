//
// wxPanel derived class where all the grids, numbers, and mines are drawn on.
// TO DO: Use RefreshRect instead of Refresh for hopefully better performance (if it helps).
//

/*************************************/
/* Includes                          */
/*************************************/

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include "Minefield.h"

#include "state/CharacterState.h"
#include "state/GameState.h"


/*************************************/
/* Definitions                       */
/*************************************/

// The overall size of the grid visible on the window
const wxUint32 Minefield::GRID_PIXEL_SIZE = 20;

// The width of the border
const wxUint32 Minefield::GRID_BORDER_WIDTH = 1;

// wxSize(240,160) - default size
Minefield::Minefield(wxWindow* parent, States & states)
    : wxWindow(parent, wxID_ANY, wxDefaultPosition), leftCurrentlyDownPos(-1,-1),
    dimensions(wxSize(12*GRID_PIXEL_SIZE, 9*GRID_PIXEL_SIZE)), mine(wxBITMAP_PNG(mine_bitmap)) {
    // Initial settings for the numbers to be drawn on the grids, and their colors
    static const GridNumberBitmapArray gridNumberBitmapArray;

    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetSize(dimensions);

    // 'Extract' the states that will be used in this class
    auto & characterState = std::get<CharacterState &>(states);
    auto & gameState = std::get<GameState &>(states);

    // Register. The function passed is what gets called upon notify all (Basically "update" function)
    gameState.Register([this](const GameState & gameState) {
        // Skip all the sizing operations, as no size change can occur during game
        if (gameState.IsReadyToPlay() && !gameState.IsInGame()) {
            // shouldDrawContents = gameState.IsReadyToPlay();
            auto newSize = (gameState.GetSize() * GRID_PIXEL_SIZE);
            dimensions.Set(newSize.x, newSize.y);
            InvalidateBestSize();
            GetParent()->Fit(); // Let parent fit all its contents.
            // This will (I think) call Minefield::DoGetBestClientSize
        }
        Refresh();
    });

    Bind(wxEVT_PAINT, [this, &gameState](wxPaintEvent & event) {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();
        if (auto context = wxGraphicsContext::Create(dc)) {
            // If during a game: Do what is in the if block
            // If not in game (default screen): Do what is in the else block
            if (gameState.IsFieldGenerated()) {
                // To do: If just the left button is being held down, just repaint that single grid being pressed.
                // Thus no need for a for loop here.
                for (auto & eachRow : gameState.GetGridMap()) {
                    for (auto & grid : eachRow) {
                        bool isPushedDown = leftCurrentlyDownPos == wxPoint(grid.x, grid.y);
                        if (isPushedDown || grid.isRevealed)
                            context->SetBrush(*wxLIGHT_GREY_BRUSH);
                        else {
                            // Fill background
                            context->SetBrush(context->CreateLinearGradientBrush(
                                    GRID_BORDER_WIDTH, GRID_BORDER_WIDTH,
                                    GRID_PIXEL_SIZE - GRID_BORDER_WIDTH - 1,
                                    GRID_PIXEL_SIZE - GRID_BORDER_WIDTH - 1,
                                    wxColour(204, 204, 204),
                                    wxColour(219, 219, 219)
                            ));
                        }
                        context->SetPen(wxPen((isPushedDown || grid.isRevealed) ? wxColour(134, 134, 134) :
                            wxColour(234, 234, 234), GRID_BORDER_WIDTH));
                        context->DrawRectangle(grid.x * GRID_PIXEL_SIZE, grid.y * GRID_PIXEL_SIZE,
                            GRID_PIXEL_SIZE, GRID_PIXEL_SIZE);

                        // Get all corner coordinates
                        wxInt32
                            xCoordLeft = grid.x * GRID_PIXEL_SIZE,
                        // -1 because 0 to BORDER_PIXEL_SIZE-1 is one grid size
                            xCoordRight =
                            grid.x * GRID_PIXEL_SIZE + GRID_PIXEL_SIZE - 1, // these -1s look sketchy
                            yCoordUp = grid.y * GRID_PIXEL_SIZE,
                            yCoordBottom = grid.y * GRID_PIXEL_SIZE + GRID_PIXEL_SIZE - 1;

                        // Stroke 'bright' line on TOP
                        context->StrokeLine(xCoordLeft, yCoordUp, xCoordRight, yCoordUp);

                        // Stroke 'bright' line on LEFT
                        context->StrokeLine(xCoordLeft, yCoordUp, xCoordLeft, yCoordBottom);

                        if (!isPushedDown && !grid.isRevealed)
                            context->SetPen(wxPen(wxColour(165, 165, 165), GRID_BORDER_WIDTH));
                        // Stroke 'dark' line on RIGHT
                        context->StrokeLine(xCoordRight, yCoordBottom, xCoordRight, yCoordUp);

                        // Stroke 'dark' line on BOTTOM
                        context->StrokeLine(xCoordRight, yCoordBottom, xCoordLeft, yCoordBottom);

                        // If already revealed && grid is surrounded by mines, number it
                        if (grid.isRevealed && grid.nSurroundingMines) {
                            context->DrawBitmap(gridNumberBitmapArray[grid.nSurroundingMines - 1],
                                    grid.x * GRID_PIXEL_SIZE, grid.y * GRID_PIXEL_SIZE,
                                    GRID_PIXEL_SIZE, GRID_PIXEL_SIZE);
                        } else if (grid.isFlagged) { // If grid is flagged, draw it
                            context->SetPen(*wxBLACK_PEN);
                            double hMargin = GRID_PIXEL_SIZE * 0.1,
                                    vMargin = GRID_PIXEL_SIZE * 0.15;
                            double flagHeight = (GRID_PIXEL_SIZE - (2 * vMargin));
                            context->SetBrush(wxBrush(wxColour(201, 100, 0)));
                            context->DrawRectangle((grid.x * GRID_PIXEL_SIZE) + hMargin,
                                                   ((grid.y + 1) * GRID_PIXEL_SIZE) - (2 * vMargin),
                                                   GRID_PIXEL_SIZE * 0.3,
                                                   vMargin); // vMargin == the height of the rectangle at the bottom
                            context->SetBrush(wxBrush(wxColour(237, 237, 237)));
                            context->DrawRectangle((grid.x * GRID_PIXEL_SIZE) + (2 * hMargin),
                                                   (grid.y * GRID_PIXEL_SIZE) + vMargin, hMargin, flagHeight - vMargin);
                            context->SetBrush(wxBrush(wxColour(255, 128, 0)));
                            auto flagPath = context->CreatePath();

                            // Draw: >
                            double histX = (grid.x * GRID_PIXEL_SIZE) + (3 * hMargin), // top left x
                                    histY = (grid.y * GRID_PIXEL_SIZE) + vMargin; // top left y
                            // move to middle right. GRID_BORDER_WIDTH is to give more space inbetween
                            flagPath.MoveToPoint(((grid.x + 1) * GRID_PIXEL_SIZE) - (hMargin + GRID_BORDER_WIDTH),
                                                 (grid.y * GRID_PIXEL_SIZE) + vMargin + (flagHeight / 4));
                            flagPath.AddLineToPoint(histX, histY);
                            double histX2 = (grid.x * GRID_PIXEL_SIZE) + (3 * hMargin), // bottom left x
                                    histY2 = (grid.y * GRID_PIXEL_SIZE) + vMargin + (flagHeight / 2); // bottom left y
                            flagPath.AddLineToPoint(histX2, histY2);
                            flagPath.CloseSubpath();

                            // Draw: |
                            flagPath.MoveToPoint(histX2, histY2); // move to bottom left
                            flagPath.AddLineToPoint(histX, histY);
                            flagPath.CloseSubpath();
                            context->DrawPath(flagPath);

                            // If game over and the current grid is flagged but has no mine, draw a cross "X" on it
                            if (!gameState.IsReadyToPlay() && !grid.isMineGrid) {
                                auto crossPath = context->CreatePath();
                                context->SetPen(wxPen(*wxRED, 2));
                                crossPath.MoveToPoint((grid.x * GRID_PIXEL_SIZE) + vMargin, // vMargin for x too
                                                      (grid.y * GRID_PIXEL_SIZE) + vMargin);
                                crossPath.AddLineToPoint(((grid.x + 1) * GRID_PIXEL_SIZE) - vMargin,
                                                         (grid.y + 1) * GRID_PIXEL_SIZE - vMargin);
                                crossPath.MoveToPoint(((grid.x + 1) * GRID_PIXEL_SIZE) - vMargin, // vMargin for x too
                                                      (grid.y * GRID_PIXEL_SIZE) + vMargin);
                                crossPath.AddLineToPoint(grid.x * GRID_PIXEL_SIZE + vMargin,
                                                         (grid.y + 1) * GRID_PIXEL_SIZE - vMargin);
                                context->StrokePath(crossPath);
                            }
                        }
                        // If game over (when game not ready to play  == !gameState.IsReadyToPlay()
                        // ...BUT minefield still should be drawn == Minefield::shouldDrawContents), show mines
                        else if (!gameState.IsReadyToPlay() && grid.isMineGrid) {
                            /*context->SetBrush(*wxBLACK_BRUSH);
                            double circleSize = GRID_PIXEL_SIZE * 0.5,
                                    margin = (GRID_PIXEL_SIZE - circleSize) / 2;
                            context->DrawEllipse(grid.x * GRID_PIXEL_SIZE + margin,
                                                 grid.y * GRID_PIXEL_SIZE + margin, circleSize, circleSize);*/
                            context->DrawBitmap(mine, grid.x * GRID_PIXEL_SIZE, grid.y * GRID_PIXEL_SIZE,
                                    GRID_PIXEL_SIZE, GRID_PIXEL_SIZE);
                            // auto path = context->CreatePath();
                            // Do the spikes later...
                        }
                    }
                }
            } else {
                context->SetBrush(*wxLIGHT_GREY_BRUSH);
                context->DrawRectangle(0, 0, GetSize().x, GetSize().y);
                context->SetFont(wxFont(wxFontInfo(7.8).Italic()), *wxBLACK);
                context->DrawText(wxString::FromUTF8("Click \"Game > New Game\" to start a round"), 5, 5);
            }
            delete context;
        }
    });

    // Make the clicked grid darker, if necessary
    Bind(wxEVT_LEFT_DOWN, [this, &characterState, &gameState](wxMouseEvent & event) {
        // If initial screen, don't do anything.
        if (!gameState.IsReadyToPlay()) return;

        auto clickedPoint = event.GetPosition() / GRID_PIXEL_SIZE;
        // Don't make it clickable too if it is flagged
        if (!gameState.GetGridMap()[clickedPoint.y][clickedPoint.x].isRevealed &&
            !gameState.GetGridMap()[clickedPoint.y][clickedPoint.x].isFlagged) {
            leftCurrentlyDownPos.x = clickedPoint.x; // * GRID_PIXEL_SIZE;
            leftCurrentlyDownPos.y = clickedPoint.y; // * GRID_PIXEL_SIZE;
            Refresh();
            //RefreshRect(wxRect((leftCurrentlyDownPos.x-1) * GRID_PIXEL_SIZE, (leftCurrentlyDownPos.y-1) * GRID_PIXEL_SIZE,
            //        GRID_PIXEL_SIZE*3, GRID_PIXEL_SIZE*3));
            // Redraw character if grid held down is not revealed yet
            characterState.ChangeStatus(CharacterState::DOWN);
            characterState.NotifyAll();
        }
    });

    Bind(wxEVT_LEFT_UP, [this, &characterState, &gameState](wxMouseEvent & event) {
        // Would be nice if, instead of checking with if statements, bind/unbind were used.
        if (!gameState.IsReadyToPlay()) return;

        leftCurrentlyDownPos.x = -1;
        leftCurrentlyDownPos.y = -1;
        // Refresh(); // Do we need this here?
        auto clickedPos = event.GetPosition() / GRID_PIXEL_SIZE;
        // Do nothing if the grid is already revealed or flagged
        if (gameState.GetGridMap()[clickedPos.y][clickedPos.x].isRevealed ||
            gameState.GetGridMap()[clickedPos.y][clickedPos.x].isFlagged) return;
        if (!gameState.IsInGame()) {
            gameState.BeginGame(clickedPos.x, clickedPos.y);
        }
        gameState.RevealGrid(clickedPos.x, clickedPos.y); // If clicked grid is mine grid, game will end at this point!
        if (gameState.AttemptToEndGame()) // Check if the game can be over now. True if game can end now.
            characterState.ChangeStatus(gameState.PlayerWins() ? CharacterState::WIN : CharacterState::LOSE);
        else
            characterState.ChangeStatus(CharacterState::NORMAL);
        // Redraw character
        characterState.NotifyAll();
        gameState.NotifyAll(); // Update all observers that the game has started
        // RootFrame especially needs to be notified for the dialog that asks for restart confirmation
    });

    Bind(wxEVT_MOTION, [this, &characterState, &gameState](wxMouseEvent & event) {
        if (!gameState.IsReadyToPlay()) return;
        // Left click button must be held down
        // IsFullySpecified must be false anyway if field not generated yet <- To do: fix this information
        if (event.LeftIsDown()) {
            auto mousePoint = event.GetPosition() / GRID_PIXEL_SIZE;
            // If currently held grid is not yet revealed AND currently held grid
            // ...is an adjacent grid, make that grid the one currently held down
            if (!gameState.GetGridMap()[mousePoint.y][mousePoint.x].isRevealed &&
                    !gameState.GetGridMap()[mousePoint.y][mousePoint.x].isFlagged &&
                    mousePoint != leftCurrentlyDownPos / GRID_PIXEL_SIZE) {
                leftCurrentlyDownPos.x = mousePoint.x; // * GRID_PIXEL_SIZE;
                leftCurrentlyDownPos.y = mousePoint.y; // * GRID_PIXEL_SIZE;
                // RefreshRect(wxRect((leftCurrentlyDownPos.x-1) * GRID_PIXEL_SIZE, (leftCurrentlyDownPos.y-1) * GRID_PIXEL_SIZE,
                //     GRID_PIXEL_SIZE*3, GRID_PIXEL_SIZE*3));
                characterState.ChangeStatus(CharacterState::DOWN);
                characterState.NotifyAll();
            } else if (gameState.GetGridMap()[mousePoint.y][mousePoint.x].isRevealed ||
                    gameState.GetGridMap()[mousePoint.y][mousePoint.x].isFlagged) {
                // If the currently held down grid is already clicked or flagged, no longer make the old one currently held down
                leftCurrentlyDownPos.x = -1;
                leftCurrentlyDownPos.y = -1;
                characterState.ChangeStatus(CharacterState::NORMAL);
                characterState.NotifyAll();
            }
            Refresh();
        }
    });

    Bind(wxEVT_LEAVE_WINDOW, [this, &characterState](wxMouseEvent & event) {
        // IsFullySpecified must be false anyway if field not generated yet
        if (event.LeftIsDown() && leftCurrentlyDownPos.IsFullySpecified()) {
            leftCurrentlyDownPos.x = -1;
            leftCurrentlyDownPos.y = -1;
            Refresh(); // Would be lovely if this was RefreshRect.
            // Revert character back to its normal state
            characterState.ChangeStatus(CharacterState::NORMAL);
            characterState.NotifyAll();
        }
    });

    // To do: Bind this after map gets generated.
    Bind(wxEVT_RIGHT_UP, [this, &characterState, &gameState](wxMouseEvent & event) {
        if (!gameState.IsReadyToPlay() || !gameState.IsInGame()) return;
        auto clickPos = event.GetPosition() / GRID_PIXEL_SIZE;
        // the clicked grid must not be already revealed,
        // ...as flagging an already pressed grid is undefined behavior
        if (!gameState.GetGridMap()[clickPos.y][clickPos.x].isRevealed) {
            gameState.ChangeFlagState(clickPos.x, clickPos.y);
            if (gameState.AttemptToEndGame()) { // Check if game can be over now
                // If game ends by flagging, it is 100% that player wins so set status to win
                characterState.ChangeStatus(CharacterState::WIN);
                characterState.NotifyAll();
            }
            gameState.NotifyAll();
        }
    });
}

wxSize Minefield::DoGetBestClientSize() const {
    return {dimensions.x, dimensions.y};
}


/**
 * GridNumberBitmapArray
 * A simple class containing an array of bitmaps of numbers to be shown on revealed grids
 *  with mines surrounding.
 */
Minefield::GridNumberBitmapArray::GridNumberBitmapArray() :
        numberBitmaps({
            wxBitmap(wxBITMAP_PNG(grid1)),
            wxBitmap(wxBITMAP_PNG(grid2)),
            wxBitmap(wxBITMAP_PNG(grid3)),
            wxBitmap(wxBITMAP_PNG(grid4)),
            wxBitmap(wxBITMAP_PNG(grid5)),
            wxBitmap(wxBITMAP_PNG(grid6)),
            wxBitmap(wxBITMAP_PNG(grid7)),
            wxBitmap(wxBITMAP_PNG(grid8))
        }) {
    // Setting color for each number bitmap.
    auto && tempImage = numberBitmaps[0].ConvertToImage();
    tempImage.SetRGB(wxRect(0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE), 255, 0, 0);
    numberBitmaps[0] = tempImage;
    tempImage = numberBitmaps[1].ConvertToImage();
    tempImage.SetRGB(wxRect(0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE), 0, 255, 0);
    numberBitmaps[1] = tempImage;
    tempImage = numberBitmaps[2].ConvertToImage();
    tempImage.SetRGB(wxRect(0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE), 0, 0, 255);
    numberBitmaps[2] = tempImage;
    tempImage = numberBitmaps[3].ConvertToImage();
    tempImage.SetRGB(wxRect(0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE), 255, 255, 0);
    numberBitmaps[3] = tempImage;
    tempImage = numberBitmaps[4].ConvertToImage();
    tempImage.SetRGB(wxRect(0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE), 120, 105, 24);
    numberBitmaps[4] = tempImage;
    tempImage = numberBitmaps[5].ConvertToImage();
    tempImage.SetRGB(wxRect(0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE), 111, 111, 111);
    numberBitmaps[5] = tempImage;
    tempImage = numberBitmaps[6].ConvertToImage();
    tempImage.SetRGB(wxRect(0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE), 73, 195, 229);
    numberBitmaps[6] = tempImage;
    tempImage = numberBitmaps[7].ConvertToImage();
    tempImage.SetRGB(wxRect(0, 0, GRID_PIXEL_SIZE, GRID_PIXEL_SIZE), 117, 9, 9);
    numberBitmaps[7] = tempImage;
}

#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <vector>
#include <raylib.h>
#include "SVM.h"

using namespace std;
using namespace stacy;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 700;

class Grid {
    using GridT = MNSTData;
public:
    Grid();
    GridT getGridOfSquares() const;

private:
    int gridSize = 28;
    int cellSize = 20;
    int gridWidth = gridSize * cellSize;
    int gridHeight = gridSize * cellSize;

    int startX = (SCREEN_WIDTH - gridWidth) / 2;  // Center horizontally
    int startY = (SCREEN_HEIGHT - gridHeight) / 2;  // Center vertically

    Vector2 cursorPos{-100.0f, -100.0f};
    Color cursorColor = BLUE;

    // Grid storage
    GridT gridOfSquares;
    vector<vector<Color>> colorGrid;

    bool AreColorsEqual(Color c1, Color c2);
    void calculateDrawing(const Vector2& mousePos, GridT& grid, vector<vector<Color>>& colorGrid, int gridSize, int cellSize, int startX, int startY);
    void eraseGrid(GridT& grid, vector<vector<Color>>& colorGrid, int gridSize);
    void drawEraseButton(Rectangle buttonRect, const char* buttonText);
    void drawEnterButton(Rectangle buttonRect, const char* buttonText);
    bool isEraseButtonClicked(Rectangle buttonRect);
    bool isDoneButtonClicked(Rectangle buttonRect);
    Color generateRandomGray();
};
#endif
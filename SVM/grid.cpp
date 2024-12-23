#include "grid.h"

Grid::Grid() : colorGrid(gridSize, vector<Color>(gridSize, WHITE)) {
    gridOfSquares.data.resize(gridSize * gridSize, 0);

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Grid");
    SetTargetFPS(120);
    HideCursor();

    Rectangle eraseButton = {SCREEN_WIDTH - 150, 10, 140, 50};
    Rectangle doneButton = {SCREEN_WIDTH - 150, 100, 140, 50};
    SetTraceLogLevel(LOG_NONE);
    while (!WindowShouldClose()) {
        cursorPos = GetMousePosition();

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            calculateDrawing(cursorPos, gridOfSquares, colorGrid, gridSize, cellSize, startX, startY);
        }
        if (isEraseButtonClicked(eraseButton)) {
            eraseGrid(gridOfSquares, colorGrid, gridSize);
        }
        if (isDoneButtonClicked(doneButton)) {
            ShowCursor();
            break;  // Exit loop when "Done" is clicked
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (int row = 0; row < gridSize; row++) {
            for (int col = 0; col < gridSize; col++) {
                int cellX = startX + col * cellSize;
                int cellY = startY + row * cellSize;

                DrawRectangle(cellX, cellY, cellSize, cellSize, colorGrid[row][col]);
                DrawRectangleLines(cellX, cellY, cellSize, cellSize, BLACK);
            }
        }

        drawEraseButton(eraseButton, "Erase All");
        drawEnterButton(doneButton, "Done");
        DrawCircleV(GetMousePosition(), 10, cursorColor);

        EndDrawing();
    }

    CloseWindow();
}
stacy::MNSTData Grid::getGridOfSquares() const {
    return gridOfSquares;
}
bool Grid::AreColorsEqual(Color c1, Color c2) {
    return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

Color Grid::generateRandomGray() {
    int grayValue = GetRandomValue(80, 200);
    return Color{(unsigned char)grayValue, (unsigned char)grayValue, (unsigned char)grayValue, 255};
}

void Grid::calculateDrawing(const Vector2& mousePos, MNSTData& grid, vector<vector<Color>>& colorGrid,
                      int gridSize, int cellSize,
                      int startX, int startY) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();

        if (mousePos.x >= startX && mousePos.x < startX + gridWidth &&
            mousePos.y >= startY && mousePos.y < startY + gridHeight) {

            int col = (mousePos.x - startX) / cellSize;
            int row = (mousePos.y - startY) / cellSize;
            int index = row * gridSize + col;

            // Stronger center pixel and smoother gradient for neighbors
            if (!AreColorsEqual(colorGrid[row][col], BLACK)) {
                grid.data[index] = 255;
                colorGrid[row][col] = BLACK;

                // Add diagonals to neighbor offsets
                const int neighborOffsets[8][2] = {
                    {0, -1}, {0, 1}, {-1, 0}, {1, 0},
                    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
                };

                for (const auto& offset : neighborOffsets) {
                    int neighborRow = row + offset[1];
                    int neighborCol = col + offset[0];

                    if (neighborRow >= 0 && neighborRow < gridSize &&
                        neighborCol >= 0 && neighborCol < gridSize) {

                        int neighborIndex = neighborRow * gridSize + neighborCol;
                        if (!AreColorsEqual(colorGrid[neighborRow][neighborCol], BLACK)) {
                            // Diagonal neighbors get lower intensity
                            bool isDiagonal = abs(offset[0]) + abs(offset[1]) == 2;
                            grid.data[neighborIndex] = isDiagonal ? 64 : 128;
                            colorGrid[neighborRow][neighborCol] = generateRandomGray();
                        }
                        }
                }
            }
            }
    }
}

void Grid::eraseGrid(MNSTData& grid, vector<vector<Color>>& colorGrid, int gridSize) {
    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            int index = row * gridSize + col;
            grid.data[index] = 0;  // Set pixel to black/zero
            colorGrid[row][col] = WHITE;
        }
    }
}
void Grid::drawEraseButton(Rectangle buttonRect, const char* buttonText) {
    DrawRectangleRec(buttonRect, GRAY);
    DrawText(buttonText, buttonRect.x + 10, buttonRect.y + 10, 20, BLACK);
}

void Grid::drawEnterButton(Rectangle buttonRect, const char* buttonText){
    DrawRectangleRec(buttonRect, GREEN);
    DrawText(buttonText, buttonRect.x + 10, buttonRect.y + 10, 20, BLACK);
}

bool Grid::isEraseButtonClicked(Rectangle buttonRect) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
           CheckCollisionPointRec(GetMousePosition(), buttonRect);
}

bool Grid::isDoneButtonClicked(Rectangle buttonRect){
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
           CheckCollisionPointRec(GetMousePosition(), buttonRect);
}
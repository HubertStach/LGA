#include "automat.h"
#include "raylib.h"
#include <cstdlib>
#include <iostream>

block::block() {
    this->state = HPP::EMPTY;
}

block::block(uint8_t initialState) {
    this->state = initialState;
}

int block::particleCount() const {
    if (isWall()) return 0;
    int count = 0;
    if (state & HPP::EAST) count++;
    if (state & HPP::NORTH) count++;
    if (state & HPP::WEST) count++;
    if (state & HPP::SOUTH) count++;
    return count;
}

Automat::Automat() {

}

void Automat::init(int x_size, int y_size, int SCREEN_HEIGHT, int SCREEN_WIDTH)
{
    this->pole.assign(x_size, std::vector<block>(y_size));

    for (int i = 0; i < x_size; ++i) {
        for (int j = 0; j < y_size; ++j) {

            bool is_border = (i == 0 || i == x_size - 1 || j == 0 || j == y_size - 1);
            bool is_middle_wall = (i > x_size / 3 && i < 2 * x_size / 3 && j == y_size / 2);

            if (is_border || is_middle_wall) {
                pole[i][j] = block(HPP::WALL);
            }
            else {
                uint8_t s = 0;

                if (std::rand() % 10 < 4) s |= HPP::EAST;
                if (std::rand() % 10 < 4) s |= HPP::NORTH;
                if (std::rand() % 10 < 4) s |= HPP::WEST;
                if (std::rand() % 10 < 4) s |= HPP::SOUTH;
                pole[i][j] = block(s);
            }
        }
    }

    int toolbar_width = 250;
    int margin = 20;

    int x_start = toolbar_width + margin;
    int x_end = SCREEN_WIDTH - margin;
    int y_start = margin;
    int y_end = SCREEN_HEIGHT - margin;

    int board_width = x_end - x_start;
    int board_height = y_end - y_start;

    if (x_size == 0 || y_size == 0) return;

    int cell_w = board_width / x_size;
    int cell_h = board_height / y_size;

    int cell_size = (cell_w < cell_h) ? cell_w : cell_h;
    if (cell_size <= 0) cell_size = 1;

    int used_width = cell_size * x_size;
    int used_height = cell_size * y_size;

    this->x_start = x_start + (board_width - used_width) / 2;
    this->y_start = y_start + (board_height - used_height) / 2;
    this->x_end = this->x_start + used_width;
    this->y_end = this->y_start + used_height;

    this->cell_size = cell_size;
    this->initialised = true;
}

void Automat::visualise()
{
    if (!initialised) return;

    int cols = this->pole.size();
    int rows = (int)pole[0].size();

    // Tło planszy
    DrawRectangle(x_start - 2, y_start - 2, (x_end - x_start) + 4, (y_end - y_start) + 4, GRAY);

    for (int i = 0; i < cols; ++i) {
        for (int j = 0; j < rows; ++j) {
            int px = x_start + i * cell_size;
            int py = y_start + j * cell_size;

            block& b = pole[i][j];
            Color col = BLACK;

            if (b.isWall()) {
                col = LIGHTGRAY;
            }
            else {
                int count = b.particleCount();
                if (count == 0) col = BLACK;
                else if (count == 1) col = DARKBLUE;
                else if (count == 2) col = BLUE;
                else if (count == 3) col = SKYBLUE;
            }

            DrawRectangle(px, py, cell_size, cell_size, col);

            if (cell_size > 4) {
                DrawRectangleLines(px, py, cell_size, cell_size, Fade(WHITE, 0.1f));
            }
        }
    }
}

void Automat::simulate_curr_state() {
    int cols = (int)this->pole.size();
    if (cols == 0) return;
    int rows = (int)this->pole[0].size();
    if (rows == 0) return;

    std::vector<std::vector<block>> next_pole;
    next_pole.assign(cols, std::vector<block>(rows));

    for (int x = 0; x < cols; ++x) {
        for (int y = 0; y < rows; ++y) {

            if (pole[x][y].isWall()) {
                next_pole[x][y] = block(HPP::WALL);
                continue;
            }

            uint8_t new_state = 0;

            // 1. ZACHÓD (Sąsiad x-1)
            int left_x = (x - 1 + cols) % cols; // Periodyczność (dla bezpieczeństwa indeksów)
            if (pole[left_x][y].isWall()) {
                // Odbicie: Jeśli my mieliśmy WEST w poprzednim kroku, teraz wraca jako EAST
                if (pole[x][y].state & HPP::WEST) new_state |= HPP::EAST;
            }
            else {
                // Normalny przelot: Bierzemy EAST od sąsiada
                if (pole[left_x][y].state & HPP::EAST) new_state |= HPP::EAST;
            }

            // 2. WSCHÓD (Sąsiad x+1)
            int right_x = (x + 1) % cols;
            if (pole[right_x][y].isWall()) {
                // Odbicie: Mieliśmy EAST, wraca WEST
                if (pole[x][y].state & HPP::EAST) new_state |= HPP::WEST;
            }
            else {
                if (pole[right_x][y].state & HPP::WEST) new_state |= HPP::WEST;
            }

            // 3. PÓŁNOC (Sąsiad y-1)
            int up_y = (y - 1 + rows) % rows;
            if (pole[x][up_y].isWall()) {
                // Odbicie: Mieliśmy NORTH, wraca SOUTH
                if (pole[x][y].state & HPP::NORTH) new_state |= HPP::SOUTH;
            }
            else {
                if (pole[x][up_y].state & HPP::SOUTH) new_state |= HPP::SOUTH;
            }

            // 4. POŁUDNIE (Sąsiad y+1)
            int down_y = (y + 1) % rows;
            if (pole[x][down_y].isWall()) {
                // Odbicie: Mieliśmy SOUTH, wraca NORTH
                if (pole[x][y].state & HPP::SOUTH) new_state |= HPP::NORTH;
            }
            else {
                if (pole[x][down_y].state & HPP::NORTH) new_state |= HPP::NORTH;
            }

            // --- Faza 2: Kolizje (Reguły HPP) ---
            // Reguła: Cząstki zderzające się czołowo (Head-on) odbijają się o 90 stopni,
            // ale tylko jeśli nie jest to zderzenie potrójne/poczwórne.

            bool col_horiz = (new_state == (HPP::EAST | HPP::WEST));
            bool col_vert = (new_state == (HPP::NORTH | HPP::SOUTH));

            if (col_horiz) {
                new_state = (HPP::NORTH | HPP::SOUTH);
            }
            else if (col_vert) {
                new_state = (HPP::EAST | HPP::WEST);
            }

            next_pole[x][y] = block(new_state);
        }
    }

    // Zamiana buforów
    this->pole = next_pole;
}
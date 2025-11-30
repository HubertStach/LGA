#pragma once

#include <vector>
#include <cstdint>

namespace HPP {
    const uint8_t EMPTY = 0;
    const uint8_t EAST = 1 << 0; // 0001
    const uint8_t NORTH = 1 << 1; // 0010
    const uint8_t WEST = 1 << 2; // 0100
    const uint8_t SOUTH = 1 << 3; // 1000
    const uint8_t WALL = 1 << 4; // 10000
}

struct block {
    uint8_t state; //kierunki plus sciany

    block();
    block(uint8_t initialState);

    // Pomocnicze metody
    bool isWall() const { return state & HPP::WALL; }
    int particleCount() const;
};

struct Automat {
    std::vector<std::vector<block>> pole;

    bool initialised = false;
    int px = 0; int py = 0;
    int cell_w = 0; int cell_h = 0;
    int x_start = 0; int x_end = 0;
    int y_start = 0; int y_end = 0;
    int cell_size = 0;

    int selected_rules = 0;
    int boundary_rule = 0;

    Automat();

    void init(int x_size, int y_size, int SCREEN_HEIGHT, int SCREEN_WIDTH);
    void visualise();
    void simulate_curr_state();
};
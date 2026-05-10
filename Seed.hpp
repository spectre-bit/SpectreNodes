/*
A wrapper around the Xoshiro256** to store a seed as a state to use it for the Seed ComfyUI node.
Copyright (C) 2026  spectre-bit

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef SEED_NODE_H
#define SEED_NODE_H
#include <cstdint>
#include "xoshiro.hpp"


class Seed {
    xso::xoshiro_4x64_star_star prng{};
    uint64_t seed = 0;

public:
    Seed(): seed{prng()} {}

    void newSeed() {
        seed = prng();
    }

    void increment() {
        ++seed;
    }

    void decrement() {
        --seed;
    }

    [[nodiscard]] uint64_t getSeed() const {
        return seed;
    }

    void setSeed(uint64_t seed) {
        this->seed = seed;
    }
};


#endif //SEED_NODE_H
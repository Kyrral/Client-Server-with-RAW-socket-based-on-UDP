#include "Randomizer.h"

Randomizer::Randomizer() {
    gen = std::mt19937(rd());
    dis = std::uniform_int_distribution<> (1, 1000);
}

int Randomizer::getRandNum() { return dis(gen); }
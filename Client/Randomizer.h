#pragma once
#include <random>

class Randomizer {
    std::random_device rd;

    std::mt19937 gen;
    std::uniform_int_distribution<> dis;

public:
    Randomizer();
    int getRandNum();
};
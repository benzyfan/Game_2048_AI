#ifndef AI_H
#define AI_H

#include <ctime>
#include <QtCore>

#include "grid.h"

struct Result {
    int move;
    double score;
    int positions;
    int cutoffs;
};



class AI {
public:
    AI(Grid *g);

    ~AI();

    int getBest();

private:
    Grid *grid;

    double eval();

    Result search(int depth, double alpha, double beta, int positions, int cutoffs);

    Result iterativeDeep();
};


#endif //AI_H

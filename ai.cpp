#include "ai.h"

AI::AI(Grid *g) {
    grid = g;
}

AI::~AI()
{
    delete grid;
    grid = NULL;
}

double AI::eval()
{
    int emptyCells = (int) grid->availableCells().size();

    double smoothWeight = 0.1,
            mono2Weight = 1.0,
            emptyWeight = 2.7,
            maxWeight = 1.0;

    return grid->smoothness() * smoothWeight
           //+ this.grid.monotonicity() * monoWeight
           //- this.grid.islands() * islandWeight
           + grid->monotonicity2() * mono2Weight
           + log(emptyCells) * emptyWeight
           + grid->maxValue() * maxWeight;

}

int AI::getBest()
{
    return iterativeDeep().move;
}

Result AI::search(int depth, double alpha, double beta, int positions, int cutoffs) {
    double bestScore;
    int bestMove = -1;
    Result result;

    // Perhaps always true?
    if (grid->playerTurn)
    {
//        cout << "Player turn" << endl;
        bestScore = alpha;

        for (int direction = 0; direction < 4; ++direction)
        {
            Grid *newGrid = grid->clone();
            if (newGrid->move(direction))
            {
                positions++;

                if (newGrid->isWin())
                {
                    return {.move = direction, .score = 10000, .positions = positions, .cutoffs = cutoffs};
                }
                //尝试引入判断失败之后的清零函数 希望可以用这种方法来防止内存爆炸
                if(newGrid->isGameover())
                {
                    return {.move = direction, .score = 10000, .positions = positions, .cutoffs = cutoffs};
                }
                AI newAI(newGrid);

                if (depth == 0)
                {
                    result = {.move = direction, .score = newAI.eval(), .positions = positions, .cutoffs = cutoffs};
                }
                else
                {
                    result = newAI.search(depth - 1, bestScore, beta, positions, cutoffs);

                    if (result.score > 9900)
                    { // win
                        result.score--; // to slightly penalize higher depth from win
                    }

                    positions = result.positions;
                    cutoffs = result.cutoffs;
                }

                if (result.score > bestScore)
                {
                    bestScore = result.score;
                    bestMove = direction;
                }
                if (bestScore > beta)
                {
                    cutoffs++;
                    return {.move = bestMove, .score = beta, .positions = positions, .cutoffs = cutoffs};
                }

            }
            else
            {
//                cout << "moved: false, direction: " << direction << endl;
            }


        }
    }
    else
    {
//        cout << "Not player turn" << endl;
        // computer's turn, we'll do heavy pruning to keep the branching factor low
        bestScore = beta;
        // try a 2 and 4 in each cell and measure how annoying it is
        // with metrics from eval
        vector<Cell> candidates;
        vector<Cell> cells = grid->availableCells();
        vector<int> scores[5];

        for (int value = 2; value <= 4; value *= 2)
        {
            for (int i = 0; i < (int)cells.size(); ++i)
            {
                scores[value].push_back(0);
                Cell cell = cells[i];
                grid->cells[cell.x][cell.y] = value;
                scores[value][i] = -grid->smoothness() + grid->islands();
                grid->cells[cell.x][cell.y] = 0;
            }
        }

        // now just pick out the most annoying moves
        int maxScore = max(*max_element(scores[2].begin(), scores[2].end()),
                           *max_element(scores[4].begin(), scores[4].end()));

        for (int value = 2; value <= 4; value *= 2)
        {
            for (int i = 0; i < (int)scores[value].size(); i++)
            {
                if (scores[value][i] == maxScore)
                {
                    candidates.push_back({cells[i].x, cells[i].y, value});
                }
            }
        }

        // search on each candidate
        for (int i=0; i< (int)candidates.size(); i++)
        {
            Cell cell = candidates[i];
            Grid *newGrid = grid->clone();
            newGrid->cells[cell.x][cell.y] = cell.value;
            newGrid->playerTurn = true;
            positions++;
            AI newAI(newGrid);
            result = newAI.search(depth, alpha, bestScore, positions, cutoffs);
            positions = result.positions;
            cutoffs = result.cutoffs;

            if (result.score < bestScore)
            {
                bestScore = result.score;
            }
            if (bestScore < alpha)
            {
                cutoffs++;
                return {.move = bestMove, .score = -1, .positions = positions, .cutoffs = cutoffs };
            }
        }
    }

    return { .move = bestMove, .score = bestScore, .positions = positions, .cutoffs = cutoffs };
}

Result AI::iterativeDeep()
{
//    time_t start = time(NULL);
    QTime start;
    start.start();
    int depth = 0;
    Result best = {.move = -1, .score = 0, .positions = 0, .cutoffs = 0 };
    do {
        Result newBest = search(depth, -10000, 10000, 0 ,0);
        if (newBest.move == -1) {
            break;
        } else {
            best = newBest;
        }
        depth++;
//        cout << "depth: " << depth << endl;
//    } while (depth < 5 && best.move != -1);
      } while (start.elapsed() < 50);
//       cout << depth << endl;
    return best;
}

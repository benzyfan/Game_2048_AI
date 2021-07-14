#ifndef GRID_H
#define GRID_H

#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

// 记录Tile的位置和值
struct Cell
{
    int x;
    int y;
    int value;
};

class Grid {
public:

    // To make them access to ai
    bool playerTurn;

    int cells[4][4];

    int oldCells[4][4];

    int visited[4][4];

    Grid(int a[4][4]);

    ~Grid();

    bool move(int direction); // 向某个方向移动

    bool isWin(); // 判断是否胜利

    bool isGameover();//判断是否失败

    int smoothness(); // 平滑性

    int monotonicity2(); // 单调性

    int maxValue(); // 最大值

    int islands(); // 孤立方块

    void mark(int x, int y, int value); // 标记方块

    vector<Cell> availableCells(); // 空闲方块

    Grid *clone(); // 复制Grid

    void update(int a[4][4]); // 更新Grid
};


#endif //GRID_H




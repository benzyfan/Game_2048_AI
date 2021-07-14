#include "grid.h"

Grid::Grid(int a[4][4])
{
    for (int x = 0; x < 4; ++x)
    {
        for (int y = 0; y < 4; ++y)
        {
            cells[x][y] = a[x][y];
        }
    }
    playerTurn = true;
}

Grid::~Grid() {
}

bool Grid::move(int direction)
{
    // 0: up, 1: right, 2:down, 3: left
    int i, j, k;
    bool moved = false;
    for (int x = 0; x < 4; ++x)
    {
        for (int y = 0; y < 4; ++y)
        {
            oldCells[x][y] = cells[x][y];
        }
    }

    switch (direction)
    {
        // 向左
        case 0:
            // 循环每一行
            for (i = 0; i < 4; i++)
            {
                /* 初始化j k为0
                 * 这里j表示要交换的数字列号
                 * k表示交换到的位置的列号
                 * */
                j = 0, k = 0;
                while (true)
                {
                    // 循环找到第一个不是0的数字对应的列号
                    while (j < 4 && cells[i][j] == 0)
                        j++;
                    // 如果超过了3则说明搜索完毕 退出循环
                    if (j > 3)
                        break;
                    // 交换两个数字
                    swap(cells[i][k], cells[i][j]);
                    //如果交换后的数字与其前一列的数字相同
                    if (k > 0 && cells[i][k] == cells[i][k - 1])
                    {
                        // 前一列的数字*2
                        cells[i][k - 1] <<= 1;
                        // 这一列的数字置为0
                        cells[i][k] = 0;
                    } else
                        k++;
                    j++;

                }
            }
            break;
            // 其余三个方向与左向类似
        case 1:
            for (i = 0; i < 4; i++) {
                j = 3, k = 3;
                while (true) {
                    while (j > -1 && cells[i][j] == 0)
                        j--;
                    if (j < 0)
                        break;
                    swap(cells[i][k], cells[i][j]);

                    if (k < 3 && cells[i][k] == cells[i][k + 1]) {
                        cells[i][k + 1] <<= 1;
                        cells[i][k] = 0;


                    } else
                        k--;
                    j--;

                }
            }
            break;
        case 2:
            for (i = 0; i < 4; i++) {
                j = 0, k = 0;
                while (true) {
                    while (j < 4 && cells[j][i] == 0)
                        j++;
                    if (j > 3)
                        break;
                    swap(cells[k][i], cells[j][i]);

                    if (k > 0 && cells[k][i] == cells[k - 1][i]) {
                        cells[k - 1][i] <<= 1;
                        cells[k][i] = 0;

                    } else
                        k++;
                    j++;

                }
            }
            break;
        case 3:
            for (i = 0; i < 4; i++) {
                j = 3, k = 3;
                while (true) {
                    while (j > -1 && cells[j][i] == 0)
                        j--;
                    if (j < 0)
                        break;
                    swap(cells[k][i], cells[j][i]);

                    if (k < 3 && cells[k][i] == cells[k + 1][i]) {
                        cells[k + 1][i] <<= 1;
                        cells[k][i] = 0;

                    } else
                        k--;
                    j--;

                }
            }
            break;
    }

    for (int x = 0; !moved && x < 4; ++x)
    {
        for (int y = 0;!moved && y < 4; ++y)
        {
            if (oldCells[x][y] != cells[x][y])
            {
                moved = true;
                playerTurn = false;
            }
        }
    }

    return moved;

}

bool Grid::isWin()
{
    for (int x = 0; x < 4; ++x)
    {
        for (int y = 0; y < 4; ++y)
        {
            if (cells[x][y] == 2048)
            {
                return true;
            }
        }

    }

    return false;
}

bool Grid::isGameover()
{
    for(int x = 0 ; x < 4;++x)
    {
        for(int y = 0 ; y < 4 ; ++y)
        {
            if(cells[x][y] == 0)
                return false;
        }
    }
    for(int i = 0; i < 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            if(cells[i][j] == cells[i+1][j] || cells[i][j] == cells[i][j+1])
            {
                //qDebug() << "2";
                return false;
            }
        }
    }
    for(int i = 0; i < 3; ++i)
    {
        if(cells[i][3] == cells[i+1][3])
        {
            return false;
        }
    }
    for(int j = 0; j < 3; ++j)
    {
        if(cells[3][j] == cells[3][j+1])
        {
            return false;
        }
    }
    return true;
}

int Grid::smoothness()
{
    int smoothness = 0;
        int targetValue;
        for (int x = 0; x < 4; ++x)
        {
            for (int y = 0; y < 4; ++y)
            {
                if (cells[x][y])
                {
                    int value = (int) (log(cells[x][y]) / log(2));

                    // row
                    for (int i = x + 1; i < 4; i++)
                    {
                        if (cells[i][y])
                        {
                            targetValue = (int) (log(cells[i][y]) / log(2));
                            smoothness -= abs(value - targetValue);
                            break;
                        }
                    }

                    for (int j = y + 1; j < 4; j++) {
                        if (cells[x][j]) {
                            targetValue = (int) (log(cells[x][j]) / log(2));
                            smoothness -= abs(value - targetValue);
                            break;
                        }
                    }
                }
            }
        }

        return smoothness;
}

int Grid::monotonicity2() {
    int totals[4] = {0, 0, 0, 0};
    for (int x = 0; x < 4; x++) {
        int current = 0;
        int next = current + 1;
        while (next < 4) {
            while (next < 4 && !cells[x][next]) {
                next++;
            }
            if (next >= 4) {
                next--;
            }
            int currentValue = (int) (cells[x][current] ?
                                      log(cells[x][current]) / log(2) :
                                      0);
            int nextValue = (int) (cells[x][next] ?
                                   log(cells[x][next]) / log(2) :
                                   0);
            if (currentValue > nextValue) {
                totals[0] += nextValue - currentValue;
            } else if (nextValue > currentValue) {
                totals[1] += currentValue - nextValue;
            }
            current = next;
            next++;
        }
    }

    for (int y = 0; y < 4; y++) {
        int current = 0;
        int next = current + 1;
        while (next < 4) {
            while (next < 4 && !cells[next][y]) {
                next++;
            }
            if (next >= 4) {
                next--;
            }
            int currentValue = (int) (cells[current][y] ?
                                      log(cells[current][y]) / log(2) :
                                      0);
            int nextValue = (int) (cells[next][y] ?
                                   log(cells[next][y]) / log(2) :
                                   0);
            if (currentValue > nextValue) {
                totals[2] += nextValue - currentValue;
            } else if (nextValue > currentValue) {
                totals[3] += currentValue - nextValue;
            }
            current = next;
            next++;
        }
    }

    return max(totals[0], totals[1]) + max(totals[2], totals[3]);
}

int Grid::maxValue() {
    int max = 0;

    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            if (cells[x][y] > max) {
                max = cells[x][y];
            }
        }

    }

    return (int) (log(max) / log(2));
}

int Grid::islands() {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            visited[x][y] = 0;
        }
    }

    int islands = 0;
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (cells[x][y] && !visited[x][y]) {
                islands++;
                mark(x, y, cells[x][y]);
            }
        }
    }
    return islands;
}

void Grid::mark(int x, int y, int value)
{
    if (x >= 0 && x < 4 && y >= 0 && y < 4 && cells[x][y] && cells[x][y] == value && !visited[x][y]) {
        visited[x][y] = 1;

        mark(x - 1, y, value);
        mark(x, y + 1, value);
        mark(x + 1, y, value);
        mark(x, y - 1, value);
    }
}

vector<Cell> Grid::availableCells()
{
    vector<Cell> cs;
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            if (!cells[x][y])
            {
                cs.push_back({.x = x, .y = y, .value = 0});
            }
        }
    }
    return cs;
}

Grid *Grid::clone() {
    Grid *newGrid = new Grid(cells);
    newGrid->playerTurn = playerTurn;
    return newGrid;
}

void Grid::update(int a[4][4]) {
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            cells[x][y] = a[x][y];
        }
    }
//    playerTurn = true;
}

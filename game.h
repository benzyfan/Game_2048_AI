#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include<QWidget>
#include<QMouseEvent>
#include<QEventLoop>
#include<QTimer>
#include<QPainter>
#include<QList>
#include <QMessageBox>

#include "ai.h"

using namespace std;

// 手势方向
enum GestureDirect {
    LEFT, RIGHT, UP, DOWN
};

// 动画类型
enum AnimationType {
    MOVE, APPEARANCE
};


// 记录动画信息
struct Animation {
    AnimationType type;
    GestureDirect direct;
    QPointF startPos;
    QPointF endPos;
    int digit0;
    int digit1;
};

class Game : public QWidget
{
    Q_OBJECT
public:
    explicit Game(QWidget *parent = 0);
    ~Game();
    // For AI to eval
//    bool playerTurn();
//    vector<Cell> availableCells();
//    double smoothness();
//    double monotonicity2();
//    double maxValue();
//    GameWidget* clone();
//    void setBoard(int b[4][4]);

    void setIsAutorunning(bool);

    void resizeEvent(QResizeEvent * );

    //游戏结束 弹出窗口
    void GameOver_Mesasage();
    //游戏胜利 弹出窗口
    void Win_Message();
    //重新开始函数 （和槽函数一致）
    void Restart_the_game();

    void stopAi_Message();

private:

    int board[4][4];
    int pastboard[4][4];
    int pastScore;
    int Score;

    QPoint startPos;
    QList<Animation> animationList;

    qreal w, h;

    //数字格子的高度与宽度
    qreal box_h, box_w;
    //窗口缩放比例
    qreal proportion_smallheight, proportion_smallwidth;
    //格子圆角的x,y
    qreal rX, rY;

    QImage *cacheImg;

    bool isAnimating;

    bool isAutorunning;

    bool isGameOver;

    bool  hasAi;

    void init_two_Block();

    void GameOver_Already();

    bool checkGameOver();

    bool checkWin();

    int getBitCount(int);

    bool playAnimation(Animation &, QPainter &);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
    void keyPressEvent(QKeyEvent *);

    QMessageBox:: StandardButton result;

    AI *ai;
    Grid *grid;

signals:
    void GestureMove(GestureDirect);

    void ScoreInc(int);

    void GameOver();

    void Win();

public slots:
    void onGestureMove(GestureDirect); // 按手势方向移动
    void restart(); // 重开游戏
    void autorun(); // AI自动运行
    void Back_one_pace();
    void autoHelp();
};

#endif // GAMEWIDGET_H

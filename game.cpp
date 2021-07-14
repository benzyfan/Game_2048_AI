#include "game.h"
#include <QDebug>
#include <QSound>

QColor Color[11] = {
    QColor::fromRgb(0xEE, 0xE4, 0xDA),//2
    QColor::fromRgb(0xED, 0xE0, 0xC8),//4
    QColor::fromRgb(0xF2, 0xB1, 0x79),//8
    QColor::fromRgb(0xEC, 0x8D, 0x54),//16
    QColor::fromRgb(0xF6, 0x7C, 0x5F),//32
    QColor::fromRgb(0xEA, 0x59, 0x37),//64
    QColor::fromRgb(0xED, 0xCF, 0x72),//128
    QColor::fromRgb(0xED, 0xCC, 0x61),//256
    QColor::fromRgb(0xE4, 0xC0, 0x2A),//512
    QColor::fromRgb(0xE2, 0xBA, 0x13),//1024
    QColor::fromRgb(0xEC, 0xC4, 0x00)//2048
};//每个数字对应的背景颜色，由截图用PS提取RGB获得
QPointF dPos[5] ;

Game::Game(QWidget *parent) : QWidget(parent)
{
    connect(this, SIGNAL(GestureMove(GestureDirect)), SLOT(onGestureMove(GestureDirect)));
    memset(board, 0, sizeof(board));
    memset(pastboard , 0, sizeof(pastboard));

    Score = 0;
    pastScore = 0;
    this ->isAnimating = false;
    this ->isAutorunning = false;
    this ->isGameOver = false;
    this ->grid = new Grid(board);
    this ->ai = new AI(grid);
    this ->cacheImg = NULL;
    this->hasAi = false;
    this ->init_two_Block();
}

void Game::init_two_Block()
{
    int i = rand()%4,j = rand()%4;
    pastboard[i][j] = board[i][j] = 2;
    i =  ( i+rand()%4 ) %4;
    j =  ( j+rand()%4 ) %4;
    while(board[i][j] != 0)
        i = rand()%4, j = rand()%4;
     pastboard[i][j] = board[i][j] = 2; //初始生成的数字均为2
    setFocus();
    update();
    //qDebug() << "Menu Worked";
}

void Game::keyPressEvent(QKeyEvent *event)
{
    if (isAnimating || isAutorunning)
        return;
    if(isGameOver)
    {
        GameOver_Already();
        return ;
    }
    if(checkWin())
    {
        Win_Message();
        return ;
    }
    switch (event->key())
    {
        case Qt::Key_A:
        case Qt::Key_Left:
            emit GestureMove(LEFT);
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            emit GestureMove(RIGHT);
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            emit GestureMove(DOWN);
            break;
        case Qt::Key_W:
        case Qt::Key_Up:
            emit GestureMove(UP);
            break;
        default:
            break;
    }
    QWidget::keyPressEvent(event);
}

void Game::mousePressEvent(QMouseEvent *e)
{
    // 获取起点坐标
    startPos = e->pos();
}

void Game::mouseReleaseEvent(QMouseEvent *e)
{
    // 如果在播放动画效果则直接退出防止重复产生手势事件
    if (isAnimating || isAutorunning)
        return;
    if(isGameOver)
    {
        GameOver_Already();
        return ;
    }
    if(checkWin())
    {
        Win_Message();
        return ;
    }
    // 根据终点坐标和起点坐标计算XY坐标的增量
    float dX = (float)(e->pos().x() - startPos.x());
    float dY = (float)(e->pos().y() - startPos.y());
    // 确定手势方向
    if (std::abs(dX) > std::abs(dY))
    {
        if (dX < 0)
            emit GestureMove(LEFT);
        else
            emit GestureMove(RIGHT);
    }
    else
    {
        if (dY < 0)
            emit GestureMove(UP);
        else
            emit GestureMove(DOWN);
    }
}

void Game::onGestureMove(GestureDirect direct)
{
    int i, j, k;
    Animation a;
    for(i = 0 ;i < 4 ;i++)
        for(j= 0;j<4;j++)
            pastboard[i][j] = board[i][j] ;
    pastScore = Score ;

    bool isMove = false, isCombine = false;
    bool isCombined[4][4];
    memset(isCombined, false, sizeof(isCombined));


    // 处理不同方向
    switch (direct)
    {
    // 向左
    case LEFT:
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
                while (j < 4 && board[i][j] == 0)
                    j++;
                // 如果超过了3则说明搜索完毕 推出循环
                if (j > 3)
                    break;

                // 交换两个数字
                qSwap(board[i][k], board[i][j]);

                if(k != j)
                    isMove = true;//最左边的那一列有没有数字，需要移动

                // 记录动画信息
                a.type = MOVE;
                a.startPos = QPointF(7 + (w + 5) * j, 7 + (h + 5) * i);
                a.endPos = QPointF(7 + (w + 5) * k, 7 + (h + 5) * i);
                a.digit0 = a.digit1 = board[i][k];
                a.direct = LEFT;
                //如果交换后的数字与其前一列的数字相同
                if (k > 0 && board[i][k] == board[i][k - 1] && !isCombined[i][k-1] )
                {
                    // 前一列的数字*2
                    board[i][k - 1] <<= 1;
                    // 这一列的数字置为0
                    board[i][k] = 0;
                    // 记录动画信息

                    isCombined[i][k-1] = true;
                    isCombine = true;

                    a.digit1 = board[i][k - 1];
                    a.endPos = QPointF(7 + (w + 5) * (k - 1), 7 + (h + 5) * i);
                    // 增加分数
                    Score += board[i][k - 1];
                    // 发射增加分数的信号
                    emit ScoreInc(Score);
//                    combine = true;
                }
                else
                    k++;
                j++;
                // 添加到动画链表
                animationList.append(a);
            }
        }
        break;
        // 其余三个方向与左向类似
    case RIGHT:
        for(i = 0; i < 4; ++i)
        {
            j = k = 3;
            while(true)
            {
                while(j >= 0 && board[i][j] == 0)
                    j--;
                if(j < 0)
                    break;
                qSwap(board[i][j], board[i][k]);
                if(k != j)
                    isMove = true;
                a.type = MOVE;
                a.startPos = QPointF(7 + (w + 5) * j, 7 + (h + 5) * i);
                a.endPos = QPointF(7 + (w + 5) * k, 7 + (h + 5) * i);
                a.digit0 = a.digit1 = board[i][k];
                a.direct = RIGHT;
                if (k < 3 && board[i][k] == board[i][k + 1]  && !isCombined[i][k+1] )
                {
                    board[i][k + 1] <<= 1;
                    board[i][k] = 0;

                    isCombined[i][k+1] = true;
                    isCombine = true;

                    a.digit1 = board[i][k + 1];
                    a.endPos = QPointF(7 + (w + 5) * (k + 1), 7 + (h + 5) * i);
                    Score += board[i][k + 1];
                    emit ScoreInc(Score);
                }
                else
                    k--;
                j--;
                animationList.append(a);
            }
        }
        break;
    case UP:
        for (i = 0; i < 4; i++)
        {
            j = 0, k = 0;
            while (true)
            {
                while (j < 4 && board[j][i] == 0)
                    j++;
                if (j > 3)
                    break;
                qSwap(board[k][i], board[j][i]);
                if(j != k)
                    isMove = true;
                a.type = MOVE;
                a.startPos = QPointF(7 + (w + 5) * i, 7 + (h + 5) * j);
                a.endPos = QPointF(7 + (w + 5) * i, 7 + (h + 5) * k);
                a.digit0 = a.digit1 = board[k][i];
                a.direct = UP;
                if (k > 0 && board[k][i] == board[k - 1][i] && !isCombined[k-1][i])
                {
                    board[k - 1][i] <<= 1;
                    board[k][i] = 0;
                    isCombined[k - 1][i] = true;
                    isCombine = true;
                    a.digit1 = board[k - 1][i];
                    a.endPos = QPointF(7 + (w + 5) * i, 7 + (h + 5) * (k - 1));
                    Score += board[k - 1][i];
                    emit ScoreInc(Score);
                }
                else
                    k++;
                j++;
                animationList.append(a);
            }
        }
        break;
    case DOWN:
        for (i = 0; i < 4; i++)
        {
            j = 3, k = 3;
            while (true)
            {
                while (j >= 0 && board[j][i] == 0)
                    j--;
                if (j < 0)
                    break;
                qSwap(board[k][i], board[j][i]);
                if(j != k)
                    isMove = true;
                a.type = MOVE;
                a.startPos = QPointF(7 + (w + 5) * i, 7 + (h + 5) * j);
                a.endPos = QPointF(7 + (w + 5) * i, 7 + (h + 5) * k);
                a.digit0 = a.digit1 = board[k][i];
                a.direct = DOWN;
                if (k < 3 && board[k][i] == board[k + 1][i])
                {
                    isCombine = true;
                    isCombined[k+1][i] = true;
                    board[k + 1][i] <<= 1;
                    board[k][i] = 0;
                    a.digit1 = board[k + 1][i];
                    a.endPos = QPointF(7 + (w + 5) * i, 7 + (h + 5) * (k + 1));
                    Score += board[k + 1][i];
                    emit ScoreInc(Score);
                }
                else
                    k--;
                j--;
                animationList.append(a);
            }
        }
        break;
    }
    int digitCount = 0 ;
    for(int p =0;p < 4; p++)
        for(int q = 0 ;q < 4 ; q++)
            if(board[p][q] != 0)
                digitCount++;
    // 如果数字未被填满
    if (digitCount != 16 && (isMove || isCombine) )
    {
        // 随机产生行号和列号
        i = rand() % 4;
        j = rand() % 4;
        // 循环直到行和列对应的元素为0
        while (board[i][j] != 0)
        {
            i = rand() % 4;
            j = rand() % 4;
        }
        // 填入2或者4
        board[i][j] = (rand() % 2 + 1) * 2;
        // 记录动画信息
        a.type = APPEARANCE;
        a.startPos = a.endPos = QPointF(7 + (w + 5) * j, 7 + (h + 5) * i);
        a.startPos += QPointF(w / 2, h / 2);
        a.digit0 = board[i][j];
    }
    // 如果数字填满了 检测游戏是否over
    if (checkGameOver())
    {
       isAutorunning = false;
       isGameOver = true;
       GameOver_Mesasage();// 如果游戏over了则发射GameOver信号
    }

    // 开始绘制动画效果
    isAnimating = true;
    // 动画列表的迭代器
    QList<Animation>::iterator it;
    // 事件循环 用于延时
    QEventLoop eventLoop;
    // 删除之前的缓存图像
    if (cacheImg)
        delete cacheImg;
    // 建立缓存图像
    if(width() == 0 && height() == 0)
        qDebug() << "error!" ;
    cacheImg = new QImage(width(), height(), QImage::Format_ARGB32);
    // 清空图像
    cacheImg->fill(0);
    // 构造一个QPainter对象
    QPainter painter(cacheImg);
    // 字体
    QFont font;
    font.setFamily("Consolas");
    font.setBold(true);
    font.setPixelSize(40*proportion_smallheight);
    painter.setFont(font);
    // 标识所有方格动画是否都播放完毕
    bool ok = false;
    while (true)
    {
        //绘制背景大方格子
        QBrush brush(QColor::fromRgb(187,173,160));
        // 使painter应用这个画刷
        painter.setBrush(brush);

        // 设置画笔为空笔 目的是使绘制的图形没有描边
        painter.setPen(Qt::NoPen);

        // 绘制一个矩形
       // painter.drawRect(2, 2, width() - 4, height() - 4);

        painter.drawRoundedRect(QRectF(2*proportion_smallwidth, 2*proportion_smallheight, width() - 4*proportion_smallwidth,
                                       height() - 4*proportion_smallheight), rX, rY);//画出圆角的背景大矩形


        // 设置画刷颜色为 RGB分量为171 165 141的颜色
        brush.setColor(QColor::fromRgb(204, 192, 180));
        // 应用这个画刷
        painter.setBrush(brush);

       for(int i = 0; i < 4; ++i)
        {
            for(int j = 0; j < 4; ++j)
            {//画圆角的小矩形
                painter.drawRoundedRect(QRectF(7 * proportion_smallwidth + (5 * proportion_smallwidth + box_w ) * j,
                                               7 * proportion_smallheight + (5 * proportion_smallheight + box_h ) * i, box_w , box_h ), rX, rY);
            }
        }
        // 假设都播放完毕
        ok = true;

        // 循环播放每个方格动画
        for (it = animationList.begin(); it != animationList.end(); it++)
            if (!playAnimation(*it, painter))
                ok = false;

        // 刷新部件
        update();

        // 全部播放完则退出
        if (ok)
            break;

        // 延时5ms
        QTimer::singleShot(1, &eventLoop, SLOT(quit()));
        eventLoop.exec();
    }
    // 播放方格出现的动画
    while (!playAnimation(a, painter))
    {
        update();
        QTimer::singleShot(1, &eventLoop, SLOT(quit()));
        eventLoop.exec();
    }
    //清除所有动画
    animationList.clear();
    //刷新当前部件
    isAnimating = false;

    // 检测游戏是否获胜
    if (checkWin())
    {
        isAutorunning = false;
        Win_Message();
    }

    update();
}

bool Game::playAnimation(Animation& a, QPainter& painter)
{

    bool ret= false;//是否是播放的动画的最后一帧
    QBrush brush(Qt::SolidPattern);
    if(a.type == MOVE)
    {
        switch(a.direct)
        {
        case LEFT:
            if(a.startPos.x() > a.endPos.x())
                a.startPos += dPos[LEFT];
            else
                a.startPos = a.endPos, ret = true;
            break;
        case RIGHT:
            if(a.startPos.x() < a.endPos.x())
                a.startPos += dPos[RIGHT];
            else
                a.startPos = a.endPos, ret = true;
            break;
        case UP:
            if(a.startPos.y() > a.endPos.y())
                a.startPos += dPos[UP];
            else
                a.startPos = a.endPos, ret = true;
            break;
        case DOWN:
            if(a.startPos.y() < a.endPos.y())
                a.startPos += dPos[DOWN];
            else
                a.startPos = a.endPos, ret = true;
            break;
        }

        if(!ret)
        {//还没移动到终点，所以数字暂时还不需要进行递增
            brush.setColor(Color[getBitCount(a.digit0)]);
            painter.setBrush(brush);
            painter.drawRoundedRect(QRectF(a.startPos.x(), a.startPos.y(), box_w, box_h), rX, rY);
            if(getBitCount(a.digit0) <= 1 )
                painter.setPen(QColor::fromRgb(119,110,101));
            else
                painter.setPen(QColor::fromRgb(255,255,255));
            painter.drawText(QRectF(a.startPos.x(), a.startPos.y(), box_w, box_h), Qt::AlignCenter, QString::number(a.digit0));
        }
        else
        {//移动到终点了，所以数字需要进行递增
            brush.setColor(Color[getBitCount(a.digit1)]);
            painter.setBrush(brush);
            painter.drawRoundedRect(QRectF(a.startPos.x(), a.startPos.y(), box_w, box_h), rX, rY);
            //350
            if(getBitCount(a.digit1) <= 1 )
                painter.setPen(QColor::fromRgb(119,110,101));
            else
                painter.setPen(QColor::fromRgb(255,255,255));
            painter.drawText(QRectF(a.startPos.x(), a.startPos.y(), box_w , box_h ), Qt::AlignCenter, QString::number(a.digit1));
        }
        painter.setPen(Qt::NoPen);
    }
    else
    {//Animation type is APPREANCE
        if(a.startPos.x() > a.endPos.x())
        {
            a.startPos += dPos[4];//这个4是出现动画对应的下标
        }
        else
        {
            a.startPos = a.endPos, ret = true;
        }
        brush.setColor(Color[getBitCount(a.digit0)]);
        painter.setBrush(brush);
        painter.drawRoundedRect(QRectF(a.startPos.x(), a.startPos.y(),//注意这里两个都是x
                            box_w - 2*(a.startPos.x() - a.endPos.x()), box_h - 2 * (a.startPos.y() - a.endPos.y())), rX, rY);
        if(getBitCount(a.digit0) <= 1 )
            painter.setPen(QColor::fromRgb(119,110,101));
        else
            painter.setPen(QColor::fromRgb(255,255,255));
        painter.drawText(QRectF(a.endPos.x(), a.endPos.y(), box_w , box_h ), Qt::AlignCenter, QString::number(a.digit0));
        painter.setPen(Qt::NoPen);
    }
    return ret;
}

void Game::paintEvent(QPaintEvent *)
{
    // 构造一个QPainter对象 使用它来进行绘图
    QPainter painter(this);

    // 如果正在播放动画效果则绘制缓存位图
    if (isAnimating)
    {
        painter.drawImage(0, 0, *cacheImg);
        return;
    }

    // 绘制底部大方格子
    QBrush brush(QColor::fromRgb(187,173,160));
    // 使painter应用这个画刷
    painter.setBrush(brush);

    // 设置画笔为空笔 目的是使绘制的图形没有描边
    painter.setPen(Qt::NoPen);

    // 绘制一个矩形

    painter.drawRoundedRect(QRectF(2*proportion_smallwidth, 2*proportion_smallheight, width() - 4*proportion_smallwidth,
                                   height() - 4*proportion_smallheight), rX, rY);//画出圆角的背景大矩形

    // 计算每个小格子的宽度和高度
    w = width() - 4, h = height() - 4;
    w = (w - 25) / 4, h = (h - 25) / 4;

    /* 构造一个字体
     * 字体名字为Consolas
     * 字体设置为粗体
     * 字体大小为40像素
     * */
    QFont font;
    font.setFamily("Consolas");
    font.setBold(true);
    font.setPixelSize(40*proportion_smallheight);
    painter.setFont(font);
    // 使painter应用这个字体

    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            if(board[i][j])
            {
                brush.setColor(Color[getBitCount(board[i][j])]);
                painter.setBrush(brush);

                painter.drawRoundedRect(QRectF(7 * proportion_smallwidth + (5 * proportion_smallwidth + box_w ) * j,
                                               7 * proportion_smallheight + (5 * proportion_smallheight + box_h ) * i, box_w , box_h ), rX, rY);
                if(getBitCount(board[i][j]) <= 1)
                    painter.setPen(QColor::fromRgb(119,110,101));
                else
                    painter.setPen(QColor::fromRgb(255,255,255));
                painter.drawText(QRectF(7*proportion_smallwidth + (5*proportion_smallwidth + box_w ) * j,
                                        7 * proportion_smallheight + (5 * proportion_smallheight + box_h) * i, box_w , box_h),
                                 Qt::AlignCenter, QString::number(board[i][j]));
                painter.setPen(Qt::NoPen);
            }
            else
            {//对于没有数字存在的小方格，直接画上方格的初始底色就可以了
                brush.setColor(QColor::fromRgb(204,192,180));
                painter.setBrush(brush);
                painter.drawRoundedRect(QRectF(7 * proportion_smallwidth + (5 * proportion_smallwidth + box_w ) * j,
                                               7 * proportion_smallheight + (5 * proportion_smallheight + box_h ) * i, box_w , box_h ), rX, rY);
            }
        }
    }
}

void Game::resizeEvent(QResizeEvent *)
{
    proportion_smallwidth = width()/400.0, proportion_smallheight = height() / 400.0;
    //计算小格子的高度以及宽度
    box_w = width() - 4 * proportion_smallwidth, box_h = height() - 4 * proportion_smallheight;
    box_w = (box_w - proportion_smallwidth * 25)/4, box_h = ( box_h - proportion_smallheight * 25)/4;//每个小格子的宽度以及高度
    rX = 15 * proportion_smallwidth, rY = 15 * proportion_smallheight;

    dPos[0] = QPointF(-15*proportion_smallwidth, 0);   //25代表方格移动的速度是每秒25个单位
    dPos[1] = QPointF(15*proportion_smallwidth, 0);    //这样每次定时器触发的时候就向左边绘制这么一个小矩形
    dPos[2] = QPointF(0, -15*proportion_smallheight);
    dPos[3] = QPointF(0, 15*proportion_smallheight);
    dPos[4] = QPointF(-2 * proportion_smallwidth, -2 * proportion_smallheight);
}

void Game::restart()
{
    if(isAutorunning == true)
    {
        isAutorunning = false ;
    }
    isGameOver = false;
    QSound * sound = new QSound(":/res/TapButtonSound.wav",this);
    sound->play();
    Score = 0;
    memset(board, 0, sizeof(board));
    memset(pastboard , 0, sizeof(pastboard));
    init_two_Block();
    emit ScoreInc(Score);
    setFocus();
    update();
}

bool Game::checkGameOver()
{
    for(int i = 0 ; i < 4 ; i++)
        for(int j = 0 ;j < 4 ; j++)
            if(board[i][j] == 0)
                return false;
    for(int i = 0; i < 3; ++i)
    {
        for(int j = 0; j < 3; ++j)
        {
            if(board[i][j] == board[i+1][j] || board[i][j] == board[i][j+1])
            {
                //qDebug() << "2";
                return false;
            }
        }
    }
    //额外判断行列 防止出现越界
    for(int i = 0; i < 3; ++i)
    {
        if(board[i][3] == board[i+1][3])
        {
            return false;
        }
    }
    for(int j = 0; j < 3; ++j)
    {
        if(board[3][j] == board[3][j+1])
        {
            //qDebug() << "4";
            return false;
        }
    }
    isAutorunning = false;
    return true;
}

bool Game::checkWin()
{
    // 循环检测是否某个方格的数字为2048
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (board[i][j] == 2048)
            {
                if (isAutorunning == true)
                    hasAi =true ;
                return true;
            }
    return false;
}

int Game::getBitCount(int n)
{
    // 循环获取数字二进制位数
    int c = -1;
    while (n >>= 1)
        c++;
    return c ;
}

void Game::Restart_the_game()
{
    if(isAutorunning == true)
    {
        isAutorunning = false ;
    }
    isGameOver = false ;
    QSound * sound = new QSound(":/res/TapButtonSound.wav",this);
    sound->play();
    pastScore = Score = 0;
    memset(board, 0, sizeof(board));
    memset(pastboard , 0 ,sizeof(pastboard));
    init_two_Block();
    emit ScoreInc(Score);
    update();
}

void Game::GameOver_Mesasage()
{
    qDebug()<<"GameOver";
    result = QMessageBox::information(this, "Fail", "你输了",QMessageBox::Retry|QMessageBox::Ok);
    switch (result)
    {
        case QMessageBox::Retry:
            //qDebug()<<"Retry";
            Restart_the_game();
            break;
        case QMessageBox::Ok:
            //qDebug()<<"Ok";
            break;
        default:
            break;
    }
}

void Game::GameOver_Already()
{
    qDebug()<<"GameOver";
    result = QMessageBox::information(this, "Fail", "你输了,请重新开始游戏!",QMessageBox::Retry|QMessageBox::Ok);
    switch (result)
    {
        case QMessageBox::Retry:
            //qDebug()<<"Retry";
            Restart_the_game();
            break;
        case QMessageBox::Ok:
            //qDebug()<<"Ok";
            break;
        default:
            break;
    }
}

void Game::Win_Message()
{
    result = QMessageBox::information(this, "Congratulations", "你赢了!",QMessageBox::Retry|QMessageBox::Ok);
    switch (result)
    {
        case QMessageBox::Retry:
            //qDebug()<<"Retry";
            Restart_the_game();
            break;
        case QMessageBox::Ok:
           // qDebug()<<"Ok";
            break;
        default:
            break;
    }
}

void Game::stopAi_Message()
{
    QMessageBox::information(this, "NO", "您已经依靠AI赢过了，不如这次试试自己来？",QMessageBox::Yes|QMessageBox::Ok);
}

void Game::Back_one_pace()
{
    if(isAutorunning == true)
    {
        isAutorunning = false ;
    }
    QSound * sound = new QSound(":/res/BackButtonSound.wav",this);
    sound->play();
    for(int i = 0 ; i < 4 ; i++)
        for(int j = 0 ; j < 4 ; j++)
        {
            board[i][j] = pastboard[i][j] ;
        }
    Score = pastScore ;
    setFocus();
    emit ScoreInc(Score);
    update();
}

// ******
// For AI
// ******
void Game::autoHelp()
{
    QSound * sound = new QSound(":/res/Helpbutton.wav",this);
    sound->play();

    if(isAutorunning == true)
        isAutorunning = false ;
    if(isGameOver == true)
    {
        GameOver_Already();
        return ;
    }
    if ( !isAnimating)
    {
        grid->update(board);
        int direction = ai->getBest();
        switch (direction)
        {
        case 0:
            emit GestureMove(LEFT);
            break;

        case 1:
            emit GestureMove(RIGHT);
            break;

        case 2:
            emit GestureMove(UP);
            break;

        case 3:
            emit GestureMove(DOWN);
            break;
        default:
            isAnimating = false;
        }
    }
}
void Game::autorun()
{
    if(hasAi == true)
    {
        stopAi_Message();
        return ;
    }
    if(isGameOver == true)
    {
        GameOver_Already();
        return ;
    }
    QSound * sound = new QSound(":/res/one.wav",this);
    sound->play();

    isAutorunning = isGameOver? false : !isAutorunning;

    while (isAutorunning == true && !isAnimating)
    {
        grid->update(board);
        int direction = ai->getBest();
        switch (direction)
        {
        case 0:
            emit GestureMove(LEFT);
            break;

        case 1:
            emit GestureMove(RIGHT);
            break;

        case 2:
            emit GestureMove(UP);
            break;

        case 3:
            emit GestureMove(DOWN);
            break;
        default:
            isAutorunning = false;
        }
    }

}

Game::~Game()
{
    cout << "Disable autorunning" << endl;
    isAutorunning = false;
    delete ai;
    delete grid;
}

void Game::setIsAutorunning(bool p)
{
    isAutorunning = p;
    qDebug() << "结束啦" ;
}

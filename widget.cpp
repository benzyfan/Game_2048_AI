#include "gui/widget.h"
#include <QApplication>
#include <QDesktopWidget>

Widget::Widget(QWidget* parent)
    : QWidget(parent)
    ,
      bPressFlag(false)
{
    // ui->setupUi(this);

    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor("#FAF8F0"));
    setPalette(pal);

    titleLabel = new QLabel(tr("2048"), this);
    titleLabel->setStyleSheet("color:#746D65");
    titleLabel->setGeometry(20,20,180,50);
    titleLabel->setFont(QFont("arial", 35, QFont::Black));

    highScore = 0;

    QFile file("scoreboard.j");
    if (file.open(QIODevice::ReadOnly))
    {
        file.read((char*)&highScore, sizeof(highScore));
        file.close();
    }

    game = new Game(this);
    game->setGeometry(2, 200, 400, 400) ;
    game->setFocus();
    connect(game, SIGNAL(ScoreInc(int)), this, SLOT(onScoreInc(int)));

    QFont font;
    font.setFamily("Consolas");
    font.setBold(false);
    font.setPixelSize(25);

    RestartButton = new QPushButton("RESTART", this);
    RestartButton->setGeometry(220,150,150,30);
    RestartButton->setFont(font);
    RestartButton->setStyleSheet(QString(ButtonType).arg(2).arg(15));
    connect(RestartButton, SIGNAL(clicked()),this->game, SLOT(restart()));
    qDebug() << "Restartbutton" ;

    highScoreLabel = new QLabel(QString("BEST:%1").arg(highScore), this);
    highScoreLabel->setGeometry(200,20,180,50);
    highScoreLabel->setFont(font);
    highScoreLabel->setAlignment(Qt::AlignCenter);
    highScoreLabel->setStyleSheet(QString(LabelType).arg(3).arg(15));

    scoreLabel = new QLabel(QString("SCORE:0"),this);//初始化设置分数label上的值为0
    scoreLabel->setGeometry(200,80,180,50);
    scoreLabel->setFont(font);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setStyleSheet(QString(LabelType).arg(3).arg(15));
    qDebug() << "Scoreevent1" ;


    BackButton = new QPushButton("BACK", this);
    BackButton->setFont(font);
    BackButton->setGeometry(30,150,150,30);
    BackButton->setStyleSheet(QString(ButtonType).arg(2).arg(15));
    BackButton->setFocusPolicy(Qt::NoFocus);
    connect(BackButton, SIGNAL(clicked()), game, SLOT(Back_one_pace()));



    AIButton = new QPushButton("AUTO", this);
    AIButton->setFont(font);
    AIButton->setGeometry(30,100,70,30);
    AIButton->setStyleSheet(QString(ButtonType).arg(2).arg(15));
    AIButton->setFocusPolicy(Qt::NoFocus);
    connect(AIButton, SIGNAL(clicked()), game, SLOT(autorun()));

    HelButton = new QPushButton("Help", this);
    HelButton->setFont(font);
    HelButton->setGeometry(110,100,70,30);
    HelButton->setStyleSheet(QString(ButtonType).arg(2).arg(15));
    HelButton->setFocusPolicy(Qt::NoFocus);
    connect(HelButton, SIGNAL(clicked()), game, SLOT(autoHelp()));

    //setFixedSize(400, 606);

    resize(404,606);

    move((QApplication::desktop()->width() - width())/2,  (QApplication::desktop()->height() - height())/2);
}

Widget::~Widget()
{
    delete AIButton;
    qDebug() << "AI delete";
    delete RestartButton;
     qDebug() << "Restart delete";
    delete scoreLabel;
     qDebug() << "score delete";
    delete highScoreLabel;
     qDebug() << "hichscore delete";
    delete BackButton;
     qDebug() << "back delete";
    qDebug() << "close game widget";
    game->setIsAutorunning(false);
    delete game;
    exit(0);
}

void Widget::onScoreInc(int score)
{
    scoreLabel->setText(QString("SCORE:%1").arg(score));
    if(score > highScore)
    {
        //首先设置最高分
        highScore = score;
        highScoreLabel->setText(QString("BEST:%1").arg(score));
        QFile file("scoreboard.j");//然后记录最高分
        file.open(QIODevice::WriteOnly);
        file.write((char*)&highScore, sizeof(highScore));
        file.close();
    }
   // qDebug() << "Scoreevent" ;
}


void Widget::resizeEvent(QResizeEvent *)
{
    // 计算宽度和高度的缩放比例
    ratioW = width() / 404.0f;
    ratioH = height() / 606.0f;
    if(ratioW == 1 && ratioH == 1)
        return ;
    // 重置子部件大小和位置
    titleLabel->setGeometry(20 * ratioW, 20 * ratioH , 180 * ratioW, 50 * ratioH);
    game->setGeometry(2 * ratioW, 200 * ratioH, 400 * ratioW, 400 * ratioH);
    RestartButton->setGeometry(220 * ratioW, 150 * ratioH, 150 * ratioW, 30 * ratioH);
    BackButton->setGeometry(30 * ratioW, 150 * ratioH, 150 * ratioW, 30 * ratioH);
    highScoreLabel->setGeometry(200 * ratioW, 20 * ratioH, 180 * ratioW, 50 * ratioH);
    scoreLabel->setGeometry(200 * ratioW, 80 * ratioH, 180 * ratioW, 50 * ratioH);
    AIButton->setGeometry(30 * ratioW, 100 * ratioH, 70 * ratioW, 30 * ratioH);
    HelButton->setGeometry(110 * ratioW, 100 * ratioH, 70 * ratioW, 30 * ratioH);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    game->setFocus();
    QWidget::keyPressEvent(event);
}
void Widget::mousePressEvent ( QMouseEvent * event)
{
    bPressFlag = true;
    dragPosition = event->pos();
    QWidget::mousePressEvent(event);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (bPressFlag)
    {
        QPoint relaPos(QCursor::pos() - dragPosition);
        move(relaPos);
    }
    QWidget::mouseMoveEvent(event);
}

void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    bPressFlag = false;
    QWidget::mouseReleaseEvent(event);
}


void Widget::onClose()
{
    cout << "onCLose()" << endl;
    game->setIsAutorunning(false);
    close();
}

void Widget::closeEvent(QCloseEvent *e)
{
    game->setIsAutorunning(false);
    game->close();
    e->accept();
    exit(0);
}

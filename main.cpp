#include "gui/widget.h"
#include <QApplication>
#include <QSound>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.setWindowTitle("2048游戏");
    w.setWindowIcon(QPixmap(":/res/title.png"));
    w.show();

    //QMessageBox::warning(NULL, "提示", "滑动鼠标或键盘WASD/↑↓←→键均可操作，凑出2048即为胜利！", QMessageBox::Yes | QMessageBox::Ok);

    //先生成界面 在播放音乐 否则会很卡
    QSound * backgroundmusic = new QSound(":/res/background.wav");
    qDebug() << "Window";
    backgroundmusic -> play();
    backgroundmusic -> setLoops(-1);//循环播放
    return a.exec();
}

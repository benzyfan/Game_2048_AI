#ifndef WIDGET_H
#define WIDGET_H

#include <QFile>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QWidget>
#include <QtGui>
#include "core\game.h"

#define LabelType "QLabel {color:rgb(255,255,255);background: rgb(187,173,160);border:%1px solid darkgray;border-radius: %250px;}"
#define ButtonType "QPushButton {color:rgb(255,255,255);background: rgb(143,122,102);border: %1px solid orange;border-radius: %2px;}"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget* parent = 0);
    ~Widget();

private:
    Ui::Widget* ui;

    QPushButton * RestartButton;
    QPushButton * AIButton;
    QPushButton * HelpButton;
    QPushButton * BackButton;
    QPushButton * HelButton;

    QLabel* titleLabel;
    QLabel* scoreLabel;
    QLabel* highScoreLabel;

    Game* game;

    qreal ratioW, ratioH;

    int highScore;
    QPoint dragPosition;
    bool bPressFlag;

protected:
    void resizeEvent(QResizeEvent*);
    void keyPressEvent(QKeyEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void closeEvent(QCloseEvent *e);

public slots:
    void onScoreInc(int);
    void onClose();
};

#endif // WIDGET_H

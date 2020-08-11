#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QDebug>
#include "iconbutton.h"
#include "game_model.h"
#include <QPushButton>
#include <QPainter>
#include <QSound>
#include <QMessageBox>
#include <QTimer>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    void init_game();//显示游戏地图

private slots:
    void on_pushButton_clicked(); //提示按钮点击槽函数
    void onIconButtonPressed(); //icon点击到响应
    void handleLinkEffect(); //实现连接效果
private:
    Ui::Widget *ui;
    Game_Model *game; // 游戏模型
    IconButton *imageButton[MAX_ROW*MAX_COL];// 图片button数组
    IconButton *preIcon, *curIcon; // 记录点击的icon
    bool isLinking; // 维持一个连接状态的标志
    QTimer *m_timer; //定时器
    int game_time; //游戏倒计时
};

#endif // WIDGET_H

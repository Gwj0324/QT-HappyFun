#include "widget.h"
#include "ui_widget.h"
#pragma execution_character_set(“utf-8”)
//全局变量
const int kIconSize=36;
const int kTopMargin=70;
const int kLeftMargin=50;
//按钮样式
const QString kIconReleasedStyle = "";
const QString kIconClickedStyle = "background-color: rgba(255, 255, 12, 161)";
const QString kIconHintStyle = "background-color: rgba(255, 0, 0, 255)";

const int kGameTimeTotal = 5 * 60 * 1000; // 总时间
const int kGameTimerInterval = 300;
const int kLinkTimerDelay = 700;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    preIcon(NULL),
    curIcon(NULL)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("连连看"));
    QSound::play(":/new/prefix1/sound/backgrand.wav");
    m_timer = new QTimer(this);
    game =new Game_Model();
    game->startGame();
    init_game();
    connect(m_timer,&QTimer::timeout,this,
            [=]()
    {
        game_time--;
        ui->label->setText(QString("%1").arg(game_time));
    });
}

Widget::~Widget()
{
    if (game)
        delete game;
    delete ui;
}

void Widget::init_game()
{
    //设置游戏倒计时
    game_time=600;
    //开启定时器
    m_timer->start(1000);
    //添加button
    for(int i=0;i<MAX_ROW*MAX_COL;i++)
    {
        imageButton[i]=new IconButton(this);
        imageButton[i]->setGeometry(kLeftMargin+(i%MAX_COL)*kIconSize,kTopMargin+(i/MAX_COL)*kIconSize,kIconSize,kIconSize);
        //设置索引
        imageButton[i]->xID=i%MAX_COL;
        imageButton[i]->yID=i/MAX_COL;
        if(game->getGameMap()[i])
        {
            // 有方块就设置图片
            QString fileString;
            fileString .sprintf(":/new/prefix1/image/%d.png",game->getGameMap()[i]);
            QPixmap iconPix(fileString);
            QIcon icon(iconPix);
            imageButton[i]->setIcon(icon);
            imageButton[i]->setIconSize(QSize(kIconSize,kIconSize));
            imageButton[i]->show();

            //添加按下的 信号与槽
            connect(imageButton[i],&IconButton::pressed,this,&Widget::onIconButtonPressed);
        }
        else
            imageButton[i]->hide();
    }
    isLinking=false;
}

void Widget::on_pushButton_clicked()
{
    //一开始不能获得提示
    for (int i = 0; i < 4;i++)
        if (game->getHint()[i] == -1)
            return;

    int x1 = game->getHint()[0];
    int y1 = game->getHint()[1];
    int x2 = game->getHint()[2];
    int y2 = game->getHint()[3];

    IconButton *srcIcon = imageButton[y1 * MAX_COL + x1];
    IconButton *dstIcon = imageButton[y2 * MAX_COL + x2];
    srcIcon->setStyleSheet(kIconHintStyle);
    dstIcon->setStyleSheet(kIconHintStyle);
}

void Widget::onIconButtonPressed()
{
    // 如果当前有方块在连接，不能点击方块
    // 因为涉及到多线，可能还要维护队列，有点复杂，就先这么简单处理一下
    if (isLinking)
    {
        // 播放音效
        QSound::play(":/new/prefix1/sound/release.wav");
        return;
    }


    // 记录当前点击的icon
    curIcon = dynamic_cast<IconButton *>(sender());

    if(!preIcon)
    {
        // 播放音效
        QSound::play(":/new/prefix1/sound/select.wav");

        // 如果单击一个icon
        curIcon->setStyleSheet(kIconClickedStyle);
        preIcon = curIcon;
    }
    else
    {
        if(curIcon != preIcon)
        {
            // 如果不是同一个button就都标记,尝试连接
            curIcon->setStyleSheet(kIconClickedStyle);
            if(game->m_remove(preIcon->xID, preIcon->yID, curIcon->xID, curIcon->yID))
            {
                // 锁住当前状态
                isLinking = true;

                // 播放音效
                QSound::play(":/new/prefix1/sound/pair.wav");

                // 延迟后实现连接效果
                QTimer::singleShot(kLinkTimerDelay, this, SLOT(handleLinkEffect()));

                // 检查是否胜利
                if (game->isWin())
                    QMessageBox::information(this, "great", "you win");

                // 每次检查一下是否僵局
                if (game->isFrozen())
                    QMessageBox::information(this, "oops", "dead game");

                int *hints = game->getHint();
            }
            else
            {
                // 播放音效
                QSound::play(":/new/prefix1/sound/release.wav");

                // 消除失败，恢复
                preIcon->setStyleSheet(kIconReleasedStyle);
                curIcon->setStyleSheet(kIconReleasedStyle);

                // 指针置空，用于下次点击判断
                preIcon = NULL;
                curIcon = NULL;
            }
        }
        else if(curIcon == preIcon)
        {
            // 播放音效
            QSound::play(":/new/prefix1/sound/release.wav");

            preIcon->setStyleSheet(kIconReleasedStyle);
            curIcon->setStyleSheet(kIconReleasedStyle);
            preIcon = NULL;
            curIcon = NULL;
        }
    }
}

void Widget::handleLinkEffect()
{
    // 消除成功，隐藏掉，并析构
    preIcon->hide();
    curIcon->hide();

    preIcon = NULL;
    curIcon = NULL;

    // 恢复状态
    isLinking = false;
}

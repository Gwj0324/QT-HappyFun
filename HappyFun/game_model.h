#ifndef GAME_MODEL_H
#define GAME_MODEL_H
#include <QTime>
#include <stdlib.h>
#include <algorithm>
// ------------ 全局变量 ------------ //
// 最大行和列数
const int MAX_ROW = 15;
const int MAX_COL = 20;
// 游戏可玩图片数量
const int MAX_ICON = 25;
class Game_Model
{
public:
    Game_Model();
    ~Game_Model();
    void startGame(); // 开始游戏
    int *getGameMap(); // 获得地图
    bool isWin(); //判断是否赢了
    int *getHint(); // 获得提示
    bool isFrozen(); // 判断是否已经成为了僵局

    bool isBlocked(int i);  //是否有障碍物
    void set_0(int i1, int i2); //将相连成功的两个gameMap[i]值设为0，即两个方块消失
    bool isSame(int x1,int y1,int x2,int y2); //判断图片是否相同
    bool horizon(int x1,int y1,int x2,int y2); //水平检测，能连接，返回true
    bool vertical(int x1,int y1,int x2,int y2); //垂直检测
    bool turn_once(int x1,int y1,int x2,int y2); //一个拐角
    bool turn_twice(int x1,int y1,int x2,int y2); //两个拐角
    bool border(int x1,int y1,int x2,int y2); //边缘情况
    bool m_remove(int x1,int y1,int x2,int y2); //判断是否可以消除
    bool isCanLink(int x1,int y1,int x2,int y2); //判断是否可以连接
private:
    // 游戏地图，存储方块，0表示消失，1-其他数字表示图片标号
    int *gameMap;
    // 游戏提示，存储2个点
    int *hintArray;
    bool isFrozenMode;
};

#endif // GAME_MODEL_H

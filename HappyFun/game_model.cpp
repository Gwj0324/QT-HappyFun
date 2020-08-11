#include "game_model.h"

Game_Model::Game_Model()
{

}

Game_Model::~Game_Model()
{
    if(gameMap)
    {
        delete(gameMap);
        delete(hintArray);
        gameMap = NULL;
    }
}

void Game_Model::startGame()
{
    gameMap = new int[MAX_ROW * MAX_COL];
    for(int i=0;i<MAX_ROW * MAX_COL;i++)
    {
        gameMap[i]=0;
    }
    hintArray = new int[4];
    for(int i=0;i<4;i++)
    {
        hintArray[i]=-1;
    }
    //填充方块标号
    int iconID=0;
    for(int i=0;i<MAX_ROW * MAX_COL;i+=2)
    {
        //每次填充连着的两个，图片用尽了就循环
        gameMap[i] = iconID % MAX_ICON + 1;
        gameMap[i + 1]=iconID % MAX_ICON + 1;
        iconID++;
    }
    //打乱
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i=0;i<MAX_ROW * MAX_COL;i++)
    {
        int randomID = qrand() % (MAX_ROW * MAX_COL);
        std::swap(gameMap[i],gameMap[randomID]);
    }
    // 初始化判断模式
    isFrozenMode = false;
}

int *Game_Model::getGameMap()
{
    return gameMap;
}

bool Game_Model::isWin()
{
    for (int i = 0; i < MAX_ROW * MAX_COL; i++)
    {
        if (gameMap[i])
            return false;
    }
    return true;
}

int *Game_Model::getHint()
{
    return hintArray;
}

bool Game_Model::isFrozen()
{
    // 暴力法，所有方块两两判断是否可以连接
    // 每次消除后做一次判断
    // 其实在这个过程中记录提示

    for (int i = 0; i < MAX_ROW * MAX_COL - 1; i++)
        for( int j = i + 1; j < MAX_ROW * MAX_COL; j++)
        {
            int x1 = i % MAX_COL;
            int y1 = i / MAX_COL;
            int x2 = j % MAX_COL;
            int y2 = j / MAX_COL;

            // 只要能找到可以连接的就不为僵局
            isFrozenMode = true;
            if (isCanLink(x1, y1, x2, y2))
            {
                // 记录第一个可以连接的hint
                hintArray[0] = x1;
                hintArray[1] = y1;
                hintArray[2] = x2;
                hintArray[3] = y2;
                isFrozenMode = false;

                return false;
            }
        }
    isFrozenMode = false;

    return true;
}

void Game_Model::set_0(int i1 ,int i2)
{
    gameMap[i1]=0;
    gameMap[i2]=0;
}

bool Game_Model::isBlocked(int i)
{
    if(gameMap[i]==0)
    {
        return false;
    }
    return true;
}

bool Game_Model::isSame(int x1, int y1, int x2, int y2)
{
    if(gameMap[y1*MAX_COL + x1]==gameMap[y2*MAX_COL + x2])
    {
        return true;
    }
    return false;
}

bool Game_Model::horizon(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2)
        {
            return false;
        }

        if (y1 != y2)
        {
            return false;
        }

        int start_x = std::min(x1, x2);
        int end_x = std::max(x1, x2);

        for (int i = start_x+1; i < end_x; i++)
        {
            if (isBlocked(y1*MAX_COL + i))
            {
                return false;
            }
        }

        return true;

}

bool Game_Model::vertical(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2)
        {
            return false;
        }

        if (x1 != x2)
        {
            return false;
        }

        int start_y = std::min(y1, y2);
        int end_y = std::max(y1, y2);

        for (int j = start_y+1; j < end_y; j++)
        {
            if (isBlocked(j*MAX_COL +x1))
            {
                return false;
            }
        }
        return true;

}

bool Game_Model::turn_once(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2)
        {
            return false;
        }

        int c_x = x1, c_y = y2;
        int d_x = x2, d_y = y1;

        int ret = false;
        if (!isBlocked(c_y*MAX_COL + c_x))
        {
            ret = vertical(x1, y1, c_x, c_y) && horizon(c_x, c_y, x2, y2);
            if (ret)
            {
                return true;
            }

        }

        if (!isBlocked(d_y*MAX_COL + d_x))
        {
            ret = horizon(x1, y1, d_x, d_y) && vertical(d_x, d_y, x2, y2);
            if (ret)
            {
                return true;
            }

        }

        return false;

}

bool Game_Model::turn_twice(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2)
    {
        return false;
    }
    int MAX_X =MAX_ROW;
    int MAX_Y =MAX_COL;
    for (int i = 0; i <= MAX_X; i++)
    {
        for (int j = 0; j <= MAX_Y; j++)
        {
            if (i != x1 && i != x2 && j != y1 && j != y2)
            {
                continue;
            }

            if ((i == x1 && j == y1) || (i == x2 && j == y2))
            {
                continue;
            }

            if (isBlocked(j*MAX_COL + i+1))
            {
                continue;
            }

            if (turn_once(x1, y1, i, j) && (horizon(i, j, x2, y2) || vertical(i, j, x2, y2)))
            {
                return true;
            }
            if (turn_once(i, j, x2, y2) && (horizon(x1, y1, i, j) || vertical(x1, y1, i, j)))
            {
                return true;
            }

        }
    }

    return false;
}

bool Game_Model::border(int x1, int y1, int x2, int y2)
{
    if (x1 == x2 && y1 == y2)
    {
        return false;
    }
    //左侧
    if((x1==0 || (!isBlocked(y1 * MAX_COL + 0)&&horizon(x1,y1,0,y1)))
            && (x2==0 || (!isBlocked(y2 * MAX_COL + 0)&&horizon(0,y2,x2,y2))))
    {
        return true;
    }
    //右侧
    if((x1==MAX_COL - 1 || (!isBlocked(y1 * MAX_COL - 1)&&horizon(x1,y1,MAX_COL - 1,y1)))
            && (x2==MAX_COL - 1 || (!isBlocked(y2 * MAX_COL - 1)&&horizon(MAX_COL - 1,y2,x2,y2))))
    {
        return true;
    }
    //上侧
    if((y1==0||(!isBlocked(x1)&&vertical(x1,y1,x1,0)))
            && (y2==0||(!isBlocked(x2)&&vertical(x2,0,x2,y2))))
    {
        return true;
    }
    //下侧
    if((y1==MAX_ROW - 1||(!isBlocked((MAX_ROW - 1)*MAX_COL +x1) &&vertical(x1,y1,x1,MAX_ROW - 1)))
            &&(y2==MAX_ROW - 1||(!isBlocked((MAX_ROW - 1)*MAX_COL +x2)&&vertical(x2,MAX_ROW - 1,x2,y2))))
    {
        return true;
    }
    return false;
}

bool Game_Model::m_remove(int x1, int y1, int x2, int y2)
{
    if(!isSame(x1,y1,x2,y2))
    {
        return false;
    }
    bool ret = false;

    ret = horizon(x1, y1, x2, y2);
    if (ret)
    {
        //值重置
        set_0(y1*MAX_COL + x1,y2*MAX_COL + x2);
        return true;
    }

    ret = vertical(x1, y1, x2, y2);
    if (ret)
    {
        set_0(y1*MAX_COL + x1,y2*MAX_COL + x2);
        return true;
    }

    ret = turn_once(x1, y1, x2, y2);
    if (ret)
    {
        set_0(y1*MAX_COL + x1,y2*MAX_COL + x2);
        return true;
    }

    ret = turn_twice(x1, y1, x2, y2);
    if (ret)
    {
        set_0(y1*MAX_COL + x1,y2*MAX_COL + x2);
        return true;
    }
    ret = border(x1, y1, x2, y2);
    if (ret)
    {
        set_0(y1*MAX_COL + x1,y2*MAX_COL + x2);
        return true;
    }

    return false;

}

bool Game_Model::isCanLink(int x1, int y1, int x2, int y2)
{
    //两个点必须要有图片
    if(!isBlocked(y1*MAX_COL + x1)||!isBlocked(y1*MAX_COL + x1))
    {
        return false;
    }
    if(!isSame(x1,y1,x2,y2))
    {
        return false;
    }
    bool ret = false;

    ret = horizon(x1, y1, x2, y2);
    if (ret)
    {
        return true;
    }

    ret = vertical(x1, y1, x2, y2);
    if (ret)
    {
        return true;
    }

    ret = turn_once(x1, y1, x2, y2);
    if (ret)
    {
        return true;
    }

    ret = turn_twice(x1, y1, x2, y2);
    if (ret)
    {
        return true;
    }
    ret = border(x1, y1, x2, y2);
    if (ret)
    {
        return true;
    }

    return false;
}

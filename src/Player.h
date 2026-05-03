/**
 * Player.h —— 玩家基类的头文件
 *
 * 功能：
 *   - 定义玩家抽象基类，所有玩家（人类/AI）均继承自此类
 *   - 每个玩家拥有一个棋子符号（CELL_X 或 CELL_O）
 *   - 纯虚函数 GetMove 由子类实现不同的走棋策略
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "Board.h"

class Player
{
public:
    Player(Cell symbol);
    virtual ~Player() {}
    Cell GetSymbol() const;
    virtual Move GetMove(const Board &board) = 0;

private:
    Cell symbol;
};

#endif
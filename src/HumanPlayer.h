/**
 * HumanPlayer.h —— 人类玩家类的头文件
 *
 * 功能：
 *   - 继承 Player 基类
 *   - 通过 SetClick 接收鼠标点击坐标
 *   - GetMove 将鼠标坐标转换为棋盘上的合法落子
 */

#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Player.h"

class HumanPlayer : public Player
{
public:
    HumanPlayer(Cell symbol);
    Move GetMove(const Board &board) override;
    void SetClick(int mx, int my, int gridX, int gridY, int cellSize);

private:
    int pendingRow;
    int pendingCol;
    bool hasPending;
};

#endif
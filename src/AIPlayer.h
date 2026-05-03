/**
 * AIPlayer.h —— AI 玩家类的头文件
 *
 * 功能：
 *   - 继承 Player 基类
 *   - 使用极小极大算法（Minimax）实现不可战胜的 AI
 *   - 先手时保证不败，后手时争取胜利或平局
 */

#ifndef AIPLAYER_H
#define AIPLAYER_H

#include "Player.h"

class AIPlayer : public Player
{
public:
    AIPlayer(Cell symbol);
    Move GetMove(const Board &board) override;

private:
    int MiniMax(Board &board, Cell currentPlayer) const;
    int Evaluate(const Board &board) const;
};

#endif
/**
 * HumanPlayer.cpp —— 人类玩家类的实现
 *
 * 工作流程：
 *   1. 外部调用 SetClick 记录鼠标点击位置
 *   2. 外部调用 GetMove 消耗此次点击：
 *      - 检查坐标是否在棋盘范围内
 *      - 检查目标格子是否为空
 *      - 合法则返回对应 Move，否则返回无效 Move
 */

#include "HumanPlayer.h"

HumanPlayer::HumanPlayer(Cell symbol)
    : Player(symbol), pendingRow(-1), pendingCol(-1), hasPending(false) {}

void HumanPlayer::SetClick(int mx, int my, int gridX, int gridY, int cellSize)
{
    int col = (mx - gridX) / cellSize;
    int row = (my - gridY) / cellSize;
    pendingRow = row;
    pendingCol = col;
    hasPending = true;
}

Move HumanPlayer::GetMove(const Board &board)
{
    if (!hasPending)
        return Move();
    hasPending = false;
    if (pendingRow < 0 || pendingRow >= BOARD_SIZE ||
        pendingCol < 0 || pendingCol >= BOARD_SIZE)
        return Move();
    if (board.Get(pendingRow, pendingCol) != CELL_EMPTY)
        return Move();
    return Move(pendingRow, pendingCol);
}
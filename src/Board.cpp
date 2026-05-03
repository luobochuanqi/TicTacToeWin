/**
 * Board.cpp —— 棋盘数据结构的实现
 *
 * 实现细节：
 *   - Reset：清空所有格子，重置落子计数
 *   - Set：写入棋子前检查坐标越界和格子是否为空
 *   - CheckWinner：逐行/逐列/对角线检查是否有连成一线
 */

#include "Board.h"

Board::Board() { Reset(); }

void Board::Reset()
{
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
            grid[r][c] = CELL_EMPTY;
    moveCount = 0;
}

Cell Board::Get(int row, int col) const
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
        return CELL_EMPTY;
    return grid[row][col];
}

bool Board::Set(int row, int col, Cell cell)
{
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
        return false;
    if (grid[row][col] != CELL_EMPTY)
        return false;
    grid[row][col] = cell;
    moveCount++;
    return true;
}

bool Board::IsFull() const
{
    return moveCount >= BOARD_SIZE * BOARD_SIZE;
}

int Board::GetMoveCount() const
{
    return moveCount;
}

Cell Board::CheckWinner() const
{
    /* 检查每一行和每一列 */
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        if (grid[i][0] != CELL_EMPTY &&
            grid[i][0] == grid[i][1] &&
            grid[i][1] == grid[i][2])
            return grid[i][0];
        if (grid[0][i] != CELL_EMPTY &&
            grid[0][i] == grid[1][i] &&
            grid[1][i] == grid[2][i])
            return grid[0][i];
    }
    /* 检查主对角线（左上到右下） */
    if (grid[0][0] != CELL_EMPTY &&
        grid[0][0] == grid[1][1] &&
        grid[1][1] == grid[2][2])
        return grid[0][0];
    /* 检查副对角线（右上到左下） */
    if (grid[0][2] != CELL_EMPTY &&
        grid[0][2] == grid[1][1] &&
        grid[1][1] == grid[2][0])
        return grid[0][2];
    return CELL_EMPTY;
}
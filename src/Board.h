/**
 * Board.h —— 棋盘数据结构的头文件
 *
 * 功能：
 *   - 定义棋盘大小常量 BOARD_SIZE（3x3）
 *   - 定义 Cell 枚举（空、X、O）
 *   - 定义 Move 结构体表示一步棋
 *   - 声明 Board 类，封装棋盘的增删查改与胜负判定
 */

#ifndef BOARD_H
#define BOARD_H

/** 棋盘尺寸（3x3 标准井字棋） */
const int BOARD_SIZE = 3;

/**
 * 棋盘格子状态枚举
 *   CELL_EMPTY = 0 —— 空格
 *   CELL_X     = 1 —— X 棋子
 *   CELL_O     = 2 —— O 棋子
 */
enum Cell
{
    CELL_EMPTY = 0,
    CELL_X = 1,
    CELL_O = 2
};

/**
 * Move 结构体 —— 表示一次落子操作
 *   row   —— 行索引（0 ~ BOARD_SIZE-1）
 *   col   —— 列索引（0 ~ BOARD_SIZE-1）
 *   IsValid() —— 判断落子坐标是否在棋盘范围内
 */
struct Move
{
    int row;
    int col;
    Move() : row(-1), col(-1) {}
    Move(int r, int c) : row(r), col(c) {}
    bool IsValid() const { return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE; }
};

/**
 * Board 类 —— 井字棋棋盘
 *
 * 职责：
 *   - 维护 3x3 二维数组作为棋盘状态
 *   - 提供 Reset / Get / Set 等基本操作
 *   - 提供 CheckWinner 胜负判定
 */
class Board
{
public:
    Board();

    void Reset();
    Cell Get(int row, int col) const;
    bool Set(int row, int col, Cell cell);
    bool IsFull() const;
    Cell CheckWinner() const;
    int GetMoveCount() const;

private:
    Cell grid[BOARD_SIZE][BOARD_SIZE];
    int moveCount;
};

#endif
/**
 * AIPlayer.cpp —— AI 玩家类的实现
 *
 * 算法：极小极大搜索（Minimax）
 *   - AI 最大化自身得分（SCORE_WIN），最小化对手得分（SCORE_LOSS）
 *   - 递归遍历所有可能的落子路径，选择最优走法
 *   - 由于井字棋状态空间较小（最多 9! 种局面），无需 Alpha-Beta 剪枝
 *
 * 得分定义：
 *   SCORE_WIN  = +10  —— AI 获胜
 *   SCORE_LOSS = -10  —— 对手获胜
 *   SCORE_DRAW =   0  —— 平局
 */

#include "AIPlayer.h"

const int SCORE_WIN = 10;
const int SCORE_LOSS = -10;
const int SCORE_DRAW = 0;

AIPlayer::AIPlayer(Cell symbol) : Player(symbol) {}

/**
 * 评估当前局面对 AI 的得分
 * @param board 当前棋盘
 * @return 局面得分
 */
int AIPlayer::Evaluate(const Board &board) const
{
    Cell winner = board.CheckWinner();
    if (winner == GetSymbol())
        return SCORE_WIN;
    if (winner != CELL_EMPTY)
        return SCORE_LOSS;
    return SCORE_DRAW;
}

/**
 * 极小极大递归搜索
 * @param board        当前棋盘副本（按值传递，不修改原始棋盘）
 * @param aiPlayer     AI 的棋子符号
 * @param currentPlayer 当前层轮到谁下棋
 * @return 该分支的评估得分
 */
static int MiniMaxRecursive(Board board, Cell aiPlayer, Cell currentPlayer)
{
    Cell winner = board.CheckWinner();
    if (winner == aiPlayer)
        return SCORE_WIN;
    if (winner != CELL_EMPTY)
        return SCORE_LOSS;
    if (board.IsFull())
        return SCORE_DRAW;

    /* AI 层取最大值，对手层取最小值 */
    int best = (currentPlayer == aiPlayer) ? -1000 : 1000;

    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            if (board.Get(r, c) != CELL_EMPTY)
                continue;

            Board nextBoard = board;
            nextBoard.Set(r, c, currentPlayer);
            Cell nextPlayer = (currentPlayer == CELL_X) ? CELL_O : CELL_X;
            int val = MiniMaxRecursive(nextBoard, aiPlayer, nextPlayer);

            if (currentPlayer == aiPlayer)
            {
                if (val > best)
                    best = val;
            }
            else
            {
                if (val < best)
                    best = val;
            }
        }
    }
    return best;
}

/**
 * AI 决策入口：遍历所有合法落子位置，选取 Minimax 评估值最高的走法
 * @param board 当前棋盘
 * @return 最优落子位置
 */
Move AIPlayer::GetMove(const Board &board)
{
    int bestVal = -1000;
    Move bestMove;

    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            if (board.Get(r, c) != CELL_EMPTY)
                continue;

            Board candidate = board;
            candidate.Set(r, c, GetSymbol());
            Cell nextPlayer = (GetSymbol() == CELL_X) ? CELL_O : CELL_X;
            int val = MiniMaxRecursive(candidate, GetSymbol(), nextPlayer);

            if (val > bestVal)
            {
                bestVal = val;
                bestMove = Move(r, c);
            }
        }
    }
    return bestMove;
}
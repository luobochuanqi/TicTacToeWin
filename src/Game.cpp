/**
 * Game.cpp —— 游戏主控制类的实现
 *
 * 游戏流程：
 *   1. 构造时默认为 PvP 模式
 *   2. 每帧调用 Update 处理 AI 延迟走棋
 *   3. HandleClick 处理鼠标点击 → 人类落子 → 检查胜负 → 切换玩家
 *   4. PvE 模式下，人类落子后启动 AI 延迟计数器，延迟结束后 AI 自动走棋
 *
 * 模式切换：
 *   - 按 1 键：切换到 PvP 模式并重置
 *   - 按 2 键：切换到 PvE 模式并重置
 *   - 按 R 键：以当前模式重置棋盘
 */

#include "Game.h"

Game::Game()
{
    Reset(MODE_PVP);
}

Game::~Game() {}

void Game::Reset(GameMode mode)
{
    board.Reset();
    gameMode = mode;
    gameState = STATE_PLAYING;
    currentPlayer = CELL_X;
    aiDelay = 0;

    playerX = std::make_unique<HumanPlayer>(CELL_X);
    if (mode == MODE_PVE)
    {
        playerO = std::make_unique<AIPlayer>(CELL_O);
    }
    else
    {
        playerO = std::make_unique<HumanPlayer>(CELL_O);
    }
}

void Game::SetMode(GameMode mode)
{
    Reset(mode);
}

void Game::HandleClick(int mx, int my, int gridX, int gridY, int cellSize)
{
    if (gameState != STATE_PLAYING)
        return;

    /* 获取当前回合的人类玩家指针（AI 回合忽略鼠标点击） */
    HumanPlayer *human = dynamic_cast<HumanPlayer *>(
        (currentPlayer == CELL_X) ? playerX.get() : playerO.get());
    if (!human)
        return;

    human->SetClick(mx, my, gridX, gridY, cellSize);
    Move move = human->GetMove(board);
    if (!move.IsValid())
        return;

    board.Set(move.row, move.col, currentPlayer);
    CheckGameOver();

    /* 若未结束且为 PvE 模式，启动 AI 延迟（视觉效果） */
    if (gameState == STATE_PLAYING && gameMode == MODE_PVE)
    {
        aiDelay = 15;
    }
}

void Game::CheckGameOver()
{
    Cell winner = board.CheckWinner();
    if (winner == CELL_X)
    {
        gameState = STATE_X_WIN;
    }
    else if (winner == CELL_O)
    {
        gameState = STATE_O_WIN;
    }
    else if (board.IsFull())
    {
        gameState = STATE_DRAW;
    }
    else
    {
        SwitchPlayer();
    }
}

void Game::SwitchPlayer()
{
    currentPlayer = (currentPlayer == CELL_X) ? CELL_O : CELL_X;
}

void Game::Update()
{
    if (gameState != STATE_PLAYING)
        return;
    if (gameMode != MODE_PVE)
        return;
    if (currentPlayer != CELL_O)
        return;

    /* AI 延迟倒计时，模拟"思考"过程 */
    if (aiDelay > 0)
    {
        aiDelay--;
        return;
    }

    DoAIMove();
}

void Game::DoAIMove()
{
    Move move = playerO->GetMove(board);
    if (!move.IsValid())
        return;

    board.Set(move.row, move.col, CELL_O);
    CheckGameOver();
}

GameMode Game::GetMode() const { return gameMode; }
GameState Game::GetState() const { return gameState; }
Cell Game::GetCurrentPlayer() const { return currentPlayer; }
const Board &Game::GetBoard() const { return board; }
int Game::GetAIDelay() const { return aiDelay; }
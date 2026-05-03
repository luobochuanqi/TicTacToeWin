/**
 * Game.h —— 游戏主控制类的头文件
 *
 * 功能：
 *   - 定义游戏模式（MODE_PVP / MODE_PVE）和游戏状态枚举
 *   - Game 类协调棋盘、玩家、事件处理与游戏流程
 */

#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Player.h"
#include "HumanPlayer.h"
#include "AIPlayer.h"
#include <memory>

/** 游戏模式枚举 */
enum GameMode
{
    MODE_PVP,  /** 双人模式 */
    MODE_PVE   /** 人机模式 */
};

/** 游戏状态枚举 */
enum GameState
{
    STATE_PLAYING,  /** 游戏进行中 */
    STATE_X_WIN,    /** X 获胜 */
    STATE_O_WIN,    /** O 获胜 */
    STATE_DRAW      /** 平局 */
};

/**
 * Game 类 —— 游戏核心控制器
 *
 * 职责：
 *   - 管理棋盘、玩家对象、游戏模式和状态
 *   - 处理鼠标点击事件并转发给对应玩家
 *   - 驱动 AI 决策与延迟动画
 *   - 检测游戏结束条件
 */
class Game
{
public:
    Game();
    ~Game();

    void Reset(GameMode mode);
    void Update();
    void HandleClick(int mx, int my, int gridX, int gridY, int cellSize);
    void SetMode(GameMode mode);

    GameMode GetMode() const;
    GameState GetState() const;
    Cell GetCurrentPlayer() const;
    const Board &GetBoard() const;
    int GetAIDelay() const;

private:
    void SwitchPlayer();
    void CheckGameOver();
    void DoAIMove();

    Board board;
    std::unique_ptr<Player> playerX;
    std::unique_ptr<Player> playerO;
    Cell currentPlayer;
    GameMode gameMode;
    GameState gameState;
    int aiDelay;
};

#endif
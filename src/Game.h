#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Player.h"
#include "HumanPlayer.h"
#include "AIPlayer.h"
#include <memory>

/** 游戏状态枚举 */
enum GameState
{
    STATE_PLAYING,
    STATE_X_WIN,
    STATE_O_WIN,
    STATE_DRAW
};

/** AI 精灵动画状态 */
enum AISpriteAnim
{
    AISA_MAKE_MOVE,
    AISA_IDLE,
    AISA_CHEER,
    AISA_SAD,
    AISA_MOVE_ANIM
};

class Game
{
public:
    Game();
    ~Game();

    void Reset();
    void Update();
    void HandleClick(int mx, int my, int gridX, int gridY, int cellSize);

    GameState GetState() const;
    Cell GetCurrentPlayer() const;
    const Board &GetBoard() const;
    int GetAIDelay() const;
    AISpriteAnim GetAISpriteAnim() const;
    int GetAISpriteFrame() const;
    WinLine GetWinLine() const;
    Move GetPendingAIMove() const;
    int GetAnimProgress() const;
    bool IsAIMoveAnimating() const;

private:
    void SwitchPlayer();
    void CheckGameOver();
    void DoAIMove();
    void AdvanceAnimFrame();

    Board board;
    std::unique_ptr<Player> playerX;
    std::unique_ptr<Player> playerO;
    Cell currentPlayer;
    GameState gameState;
    int aiDelay;

    AISpriteAnim aiSpriteAnim;
    int aiSpriteFrame;
    int aiSpriteTimer;

    Move pendingAIMove;
    int animProgress;
    static const int ANIM_TOTAL_FRAMES = 25;
};

#endif

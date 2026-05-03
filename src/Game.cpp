#include "Game.h"

static const int SPRITE_FRAME_DELAY = 15;

Game::Game()
{
    Reset();
}

Game::~Game() {}

void Game::Reset()
{
    board.Reset();
    gameState = STATE_PLAYING;
    currentPlayer = CELL_X;
    aiDelay = 0;
    aiSpriteAnim = AISA_IDLE;
    aiSpriteFrame = 0;
    aiSpriteTimer = 0;

    playerX = std::make_unique<HumanPlayer>(CELL_X);
    playerO = std::make_unique<AIPlayer>(CELL_O);
}

void Game::HandleClick(int mx, int my, int gridX, int gridY, int cellSize)
{
    if (gameState != STATE_PLAYING)
        return;

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

    if (gameState == STATE_PLAYING)
    {
        aiDelay = 15;
        aiSpriteAnim = AISA_MAKE_MOVE;
        aiSpriteFrame = 0;
        aiSpriteTimer = 0;
    }
}

void Game::CheckGameOver()
{
    Cell winner = board.CheckWinner();
    if (winner == CELL_X)
    {
        gameState = STATE_X_WIN;
        aiSpriteAnim = AISA_SAD;
        aiSpriteFrame = 0;
        aiSpriteTimer = 0;
    }
    else if (winner == CELL_O)
    {
        gameState = STATE_O_WIN;
        aiSpriteAnim = AISA_CHEER;
        aiSpriteFrame = 0;
        aiSpriteTimer = 0;
    }
    else if (board.IsFull())
    {
        gameState = STATE_DRAW;
        aiSpriteAnim = AISA_SAD;
        aiSpriteFrame = 0;
        aiSpriteTimer = 0;
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
    AdvanceAnimFrame();

    if (gameState != STATE_PLAYING)
        return;
    if (currentPlayer != CELL_O)
        return;

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

    if (gameState == STATE_PLAYING)
    {
        aiSpriteAnim = AISA_IDLE;
        aiSpriteFrame = 0;
        aiSpriteTimer = 0;
    }
}

void Game::AdvanceAnimFrame()
{
    if (++aiSpriteTimer >= SPRITE_FRAME_DELAY)
    {
        aiSpriteTimer = 0;
        aiSpriteFrame = (aiSpriteFrame + 1) % 5;
    }
}

GameState Game::GetState() const { return gameState; }
Cell Game::GetCurrentPlayer() const { return currentPlayer; }
const Board &Game::GetBoard() const { return board; }
int Game::GetAIDelay() const { return aiDelay; }
AISpriteAnim Game::GetAISpriteAnim() const { return aiSpriteAnim; }
int Game::GetAISpriteFrame() const { return aiSpriteFrame; }

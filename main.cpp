#define GAMELIB_IMPLEMENTATION
#include "lib/GameLib.h"
#include "src/Game.h"

/** 窗口宽度 */
const int WIN_W = 640;
/** 窗口高度 */
const int WIN_H = 480;
/** 每个格子的像素大小 */
const int CELL = 100;

/** 棋盘左上角 X 坐标 */
const int GRID_X = 70;
/** 棋盘左上角 Y 坐标 */
const int GRID_Y = 70;

/** Kirby 绘制尺寸（缩放后） */
const int KIRBY_DW = 180;
const int KIRBY_DH = 200;

static void DrawGrid(GameLib &gl)
{
    for (int i = 1; i < BOARD_SIZE; ++i)
    {
        int x = GRID_X + i * CELL;
        gl.DrawLine(x, GRID_Y, x, GRID_Y + BOARD_SIZE * CELL, COLOR_WHITE);
        int y = GRID_Y + i * CELL;
        gl.DrawLine(GRID_X, y, GRID_X + BOARD_SIZE * CELL, y, COLOR_WHITE);
    }
}

static void DrawMark(GameLib &gl, int row, int col, Cell cell)
{
    int cx = GRID_X + col * CELL + CELL / 2;
    int cy = GRID_Y + row * CELL + CELL / 2;
    int r = CELL / 3;

    if (cell == CELL_X)
    {
        int off = (int)(r * 0.7f);
        gl.DrawLine(cx - off, cy - off, cx + off, cy + off, COLOR_SKY_BLUE);
        gl.DrawLine(cx + off, cy - off, cx - off, cy + off, COLOR_SKY_BLUE);
    }
    else
    {
        gl.DrawCircle(cx, cy, r, COLOR_GOLD);
    }
}

static void DrawBoard(GameLib &gl, const Board &board)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            Cell cell = board.Get(r, c);
            if (cell != CELL_EMPTY)
                DrawMark(gl, r, c, cell);
        }
}

int main()
{
    GameLib gl;
    gl.Open(WIN_W, WIN_H, "Tic Tac Toe", true);

    Game game;

    int kirby = gl.LoadSprite("assets/sprites/Kirby.png");
    int kirbyFW = (kirby >= 0) ? gl.GetSpriteWidth(kirby) / 5 : 0;
    int kirbyFH = (kirby >= 0) ? gl.GetSpriteHeight(kirby) / 4 : 0;

    gl.ShowFps(true);

    while (!gl.IsClosed())
    {
        if (gl.IsKeyPressed(KEY_R))
            game.Reset();

        if (gl.IsMousePressed(MOUSE_LEFT))
            game.HandleClick(gl.GetMouseX(), gl.GetMouseY(),
                             GRID_X, GRID_Y, CELL);

        game.Update();

        /* --- 渲染 --- */
        gl.Clear(COLOR_BLACK);
        DrawGrid(gl);
        DrawBoard(gl, game.GetBoard());

        /* --- 绘制 Kirby 精灵 --- */
        if (kirby >= 0)
        {
            int frameIndex = (int)game.GetAISpriteAnim() * 5 + game.GetAISpriteFrame();
            int spriteX = GRID_X + BOARD_SIZE * CELL + 30;
            int spriteY = GRID_Y;
            gl.DrawSpriteFrameScaled(kirby, spriteX, spriteY,
                                     kirbyFW, kirbyFH, frameIndex,
                                     KIRBY_DW, KIRBY_DH,                                      SPRITE_ALPHA);
        }

        /* --- 显示游戏状态信息 --- */
        GameState state = game.GetState();
        if (state == STATE_PLAYING)
        {
            Cell cur = game.GetCurrentPlayer();
            const char *name = (cur == CELL_X) ? "X" : "O";
            uint32_t col = (cur == CELL_X) ? COLOR_SKY_BLUE : COLOR_GOLD;
            gl.DrawText(10, 10, "Turn: ", COLOR_WHITE);
            gl.DrawText(58, 10, name, col);

            if (cur == CELL_O)
                gl.DrawText(80, 10, "- AI thinking...", COLOR_GRAY);
            else
                gl.DrawText(80, 10, "- click a cell", COLOR_GRAY);
        }
        else
        {
            const char *msg;
            uint32_t col;
            if (state == STATE_X_WIN)
            {
                msg = "You win!";
                col = COLOR_SKY_BLUE;
            }
            else if (state == STATE_O_WIN)
            {
                msg = "Kirby wins!";
                col = COLOR_GOLD;
            }
            else
            {
                msg = "Draw!";
                col = COLOR_GRAY;
            }

            int tw = 8 * (int)strlen(msg);
            gl.DrawText((WIN_W - tw) / 2, GRID_Y + BOARD_SIZE * CELL + 30, msg, col);
        }

        gl.DrawText(10, WIN_H - 20, "R = restart", COLOR_DARK_GRAY);
        gl.Update();
        gl.WaitFrame(60);
    }

    if (kirby >= 0)
        gl.FreeSprite(kirby);
    return 0;
}

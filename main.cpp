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
        gl.DrawLine(x, GRID_Y, x, GRID_Y + BOARD_SIZE * CELL, COLOR_BLACK);
        int y = GRID_Y + i * CELL;
        gl.DrawLine(GRID_X, y, GRID_X + BOARD_SIZE * CELL, y, COLOR_BLACK);
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

    // 从 exe 所在目录反推项目根目录，无论从哪里启动都能找到素材
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    wchar_t *lastSep = wcsrchr(exePath, L'\\');
    if (lastSep) *lastSep = L'\0';                 // 去掉 exe 文件名
    if (lastSep) lastSep = wcsrchr(exePath, L'\\');
    if (lastSep) *lastSep = L'\0';                 // 再上一级 → 项目根目录
    wcscat(exePath, L"\\assets\\sprites\\Kirby.png");

    char kirbyPath[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, exePath, -1, kirbyPath, MAX_PATH, NULL, NULL);
    int kirby = gl.LoadSprite(kirbyPath);
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
        gl.Clear(COLOR_WHITE);
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
            gl.DrawTextFont(10, 8, "\xe5\x9b\x9e\xe5\x90\x88: ", COLOR_BLACK, 16);
            gl.DrawTextFont(58, 8, name, col, 16);

            if (cur == CELL_O)
                gl.DrawTextFont(80, 8, "- AI \xe6\x80\x9d\xe8\x80\x83\xe4\xb8\xad...", COLOR_DARK_GRAY, 16);
            else
                gl.DrawTextFont(80, 8, "- \xe7\x82\xb9\xe5\x87\xbb\xe6\xa0\xbc\xe5\xad\x90\xe8\x90\xbd\xe5\xad\x90", COLOR_DARK_GRAY, 16);
        }
        else
        {
            const char *msg;
            uint32_t col;
            int fontSize = 24;
            if (state == STATE_X_WIN)
            {
                msg = "\xe4\xbd\xa0\xe8\xb5\xa2\xe4\xba\x86!";
                col = COLOR_SKY_BLUE;
            }
            else if (state == STATE_O_WIN)
            {
                msg = "\xe5\x8d\xa1\xe6\xaf\x94\xe8\xb5\xa2\xe4\xba\x86!";
                col = COLOR_GOLD;
            }
            else
            {
                msg = "\xe5\xb9\xb3\xe5\xb1\x80!";
                col = COLOR_DARK_GRAY;
            }

            int tw = gl.GetTextWidthFont(msg, fontSize);
            gl.DrawTextFont((WIN_W - tw) / 2, GRID_Y + BOARD_SIZE * CELL + 24, msg, col, fontSize);
        }

        gl.DrawTextFont(10, WIN_H - 22, "\xe6\x8c\x89 R \xe9\x87\x8d\xe6\x96\xb0\xe5\xbc\x80\xe5\xa7\x8b", COLOR_DARK_GRAY, 14);
        gl.Update();
        gl.WaitFrame(60);
    }

    if (kirby >= 0)
        gl.FreeSprite(kirby);
    return 0;
}

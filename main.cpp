#define GAMELIB_IMPLEMENTATION
#include "lib/GameLib.h"
#include "src/Game.h"

const int WIN_W = 640;
const int WIN_H = 480;
const int CELL = 100;

const int GRID_X = 70;
const int GRID_Y = 70;

const int KIRBY_DW = 180;
const int KIRBY_DH = 200;

static wchar_t assetDir[MAX_PATH];

static void GetAssetPath(const wchar_t *filename, char *out, int outSize)
{
    wchar_t wpath[MAX_PATH];
    wcscpy(wpath, assetDir);
    wcscat(wpath, filename);
    WideCharToMultiByte(CP_UTF8, 0, wpath, -1, out, outSize, NULL, NULL);
}

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

static void DrawMark(GameLib &gl, int row, int col, Cell cell, int starSprite)
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
    else if (starSprite >= 0)
    {
        int sz = CELL * 2 / 3;
        gl.DrawSpriteScaled(starSprite, cx - sz / 2, cy - sz / 2, sz, sz, SPRITE_ALPHA);
    }
    else
    {
        gl.DrawCircle(cx, cy, r, COLOR_GOLD);
    }
}

static void DrawBoard(GameLib &gl, const Board &board, int starSprite)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            Cell cell = board.Get(r, c);
            if (cell != CELL_EMPTY)
                DrawMark(gl, r, c, cell, starSprite);
        }
}

int main()
{
    GameLib gl;
    gl.Open(WIN_W, WIN_H, "Tic Tac Toe", true);

    Game game;

    GetModuleFileNameW(NULL, assetDir, MAX_PATH);
    wchar_t *lastSep = wcsrchr(assetDir, L'\\');
    if (lastSep) *lastSep = L'\0';
    if (lastSep) lastSep = wcsrchr(assetDir, L'\\');
    if (lastSep) *lastSep = L'\0';
    wcscat(assetDir, L"\\assets\\sprites\\");

    char buf[MAX_PATH];

    GetAssetPath(L"kirby.png", buf, sizeof(buf));
    int kirby = gl.LoadSprite(buf);
    int kirbyFW = (kirby >= 0) ? gl.GetSpriteWidth(kirby) / 5 : 0;
    int kirbyFH = (kirby >= 0) ? gl.GetSpriteHeight(kirby) / 4 : 0;

    GetAssetPath(L"open.png", buf, sizeof(buf));
    int splash = gl.LoadSprite(buf);

    GetAssetPath(L"star.png", buf, sizeof(buf));
    int star = gl.LoadSprite(buf);

    gl.ShowFps(true);

    int splashPhase = 0;
    int splashTimer = 0;

    while (!gl.IsClosed())
    {
        /* ---------- 开场动画 ---------- */
        if (splashPhase < 2)
        {
            if (splash >= 0)
                gl.DrawSpriteScaled(splash, 0, 0, WIN_W, WIN_H, SPRITE_ALPHA);

            if (splashPhase == 0)
            {
                if (++splashTimer >= 60)
                {
                    splashPhase = 1;
                    splashTimer = 0;
                }
            }
            else
            {
                splashTimer += 5;
                if (splashTimer < 255)
                    gl.FillRect(0, 0, WIN_W, WIN_H, COLOR_ARGB(splashTimer, 255, 255, 255));
                else
                    splashPhase = 2;
            }

            gl.Update();
            gl.WaitFrame(60);
            continue;
        }

        /* ---------- 正常游戏 ---------- */
        if (gl.IsKeyPressed(KEY_R))
            game.Reset();

        if (gl.IsMousePressed(MOUSE_LEFT))
            game.HandleClick(gl.GetMouseX(), gl.GetMouseY(),
                             GRID_X, GRID_Y, CELL);

        game.Update();

        gl.Clear(COLOR_WHITE);
        DrawGrid(gl);
        DrawBoard(gl, game.GetBoard(), star);

        if (kirby >= 0)
        {
            int frameIndex = (int)game.GetAISpriteAnim() * 5 + game.GetAISpriteFrame();
            int spriteX = GRID_X + BOARD_SIZE * CELL + 30;
            int spriteY = GRID_Y;
            gl.DrawSpriteFrameScaled(kirby, spriteX, spriteY,
                                     kirbyFW, kirbyFH, frameIndex,
                                     KIRBY_DW, KIRBY_DH, SPRITE_ALPHA);
        }

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

    if (kirby >= 0) gl.FreeSprite(kirby);
    if (splash >= 0) gl.FreeSprite(splash);
    if (star >= 0) gl.FreeSprite(star);
    return 0;
}

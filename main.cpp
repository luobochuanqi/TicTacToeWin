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

static int CreateXSprite(GameLib &gl)
{
    int id = gl.CreateSprite(CELL, CELL);
    if (id < 0) return -1;

    int t = CELL / 8;
    for (int y = 0; y < CELL; ++y)
    {
        for (int x = 0; x < CELL; ++x)
        {
            bool onDiag1 = (abs(x - y) <= t);
            bool onDiag2 = (abs(x + y - (CELL - 1)) <= t);
            uint32_t color = (onDiag1 || onDiag2) ? COLOR_SKY_BLUE : COLOR_TRANSPARENT;
            gl.SetSpritePixel(id, x, y, color);
        }
    }
    return id;
}

static void DrawThickLine(GameLib &gl, int x1, int y1, int x2, int y2, uint32_t color, int thickness)
{
    float dx = (float)(x2 - x1);
    float dy = (float)(y2 - y1);
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 1.0f) return;
    float px = -dy / len;
    float py = dx / len;

    for (int i = -thickness / 2; i <= thickness / 2; ++i)
    {
        int ox = (int)(px * i + 0.5f);
        int oy = (int)(py * i + 0.5f);
        gl.DrawLine(x1 + ox, y1 + oy, x2 + ox, y2 + oy, color);
    }
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

static void DrawMark(GameLib &gl, int row, int col, Cell cell, int xSprite, int starSprite)
{
    int cx = GRID_X + col * CELL + CELL / 2;
    int cy = GRID_Y + row * CELL + CELL / 2;
    int sz = CELL * 2 / 3;

    if (cell == CELL_X && xSprite >= 0)
    {
        gl.DrawSpriteScaled(xSprite, cx - sz / 2, cy - sz / 2, sz, sz, SPRITE_ALPHA);
    }
    else if (cell == CELL_O && starSprite >= 0)
    {
        gl.DrawSpriteScaled(starSprite, cx - sz / 2, cy - sz / 2, sz, sz, SPRITE_ALPHA);
    }
}

static void DrawBoard(GameLib &gl, const Board &board, int xSprite, int starSprite)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            Cell cell = board.Get(r, c);
            if (cell != CELL_EMPTY)
                DrawMark(gl, r, c, cell, xSprite, starSprite);
        }
}

static void DrawWinLine(GameLib &gl, const WinLine &wl)
{
    if (!wl.IsWin()) return;

    int cx1 = GRID_X + wl.c1 * CELL + CELL / 2;
    int cy1 = GRID_Y + wl.r1 * CELL + CELL / 2;
    int cx2 = GRID_X + wl.c2 * CELL + CELL / 2;
    int cy2 = GRID_Y + wl.r2 * CELL + CELL / 2;

    uint32_t color = (wl.winner == CELL_X) ? COLOR_SKY_BLUE : COLOR_GOLD;
    DrawThickLine(gl, cx1, cy1, cx2, cy2, color, 5);
}

static void DrawHoverPreview(GameLib &gl, const Game &game, int xSprite, int starSprite)
{
    if (game.GetState() != STATE_PLAYING) return;
    if (game.GetCurrentPlayer() != CELL_X) return;

    int mx = gl.GetMouseX();
    int my = gl.GetMouseY();
    int col = (mx - GRID_X) / CELL;
    int row = (my - GRID_Y) / CELL;

    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return;
    if (game.GetBoard().Get(row, col) != CELL_EMPTY) return;

    int cx = GRID_X + col * CELL + CELL / 2;
    int cy = GRID_Y + row * CELL + CELL / 2;
    int sz = CELL * 2 / 3;
    uint32_t hintColor = COLOR_ARGB(90, 135, 206, 235);
    gl.FillRect(cx - sz / 2, cy - sz / 2, sz, sz, hintColor);
    if (xSprite >= 0)
        gl.DrawSpriteScaled(xSprite, cx - sz / 2, cy - sz / 2, sz, sz, SPRITE_ALPHA);
}

static void DrawAIFlyingPiece(GameLib &gl, const Game &game, int starSprite)
{
    if (!game.IsAIMoveAnimating() || starSprite < 0) return;

    Move target = game.GetPendingAIMove();
    int progress = game.GetAnimProgress();
    int total = 25;
    float t = (float)progress / (float)total;
    t = 1.0f - (1.0f - t) * (1.0f - t);

    int fromX = GRID_X + BOARD_SIZE * CELL + 30 + KIRBY_DW / 2;
    int fromY = GRID_Y + 30;
    int toX = GRID_X + target.col * CELL + CELL / 2;
    int toY = GRID_Y + target.row * CELL + CELL / 2;

    int curX = (int)(fromX + (toX - fromX) * t);
    int curY = (int)(fromY + (toY - fromY) * t);

    int startSz = CELL / 3;
    int endSz = CELL * 2 / 3;
    int sz = (int)(startSz + (endSz - startSz) * t);

    gl.DrawSpriteScaled(starSprite, curX - sz / 2, curY - sz / 2, sz, sz, SPRITE_ALPHA);
}

static void DrawPlayAgainButton(GameLib &gl, bool &clicked)
{
    int btnW = 140;
    int btnH = 34;
    int btnX = (WIN_W - btnW) / 2;
    int btnY = GRID_Y + BOARD_SIZE * CELL + 60;

    int mx = gl.GetMouseX();
    int my = gl.GetMouseY();
    bool hover = gl.PointInRect(mx, my, btnX, btnY, btnW, btnH);

    uint32_t bgColor = hover ? COLOR_ARGB(255, 70, 130, 220) : COLOR_SKY_BLUE;
    gl.FillRect(btnX, btnY, btnW, btnH, bgColor);
    gl.DrawRect(btnX - 1, btnY - 1, btnW + 2, btnH + 2, COLOR_DARK_BLUE);

    const char *text = "Play Again";
    int tw = gl.GetTextWidthFont(text, 18);
    int th = gl.GetTextHeightFont(text, 18);
    gl.DrawTextFont(btnX + (btnW - tw) / 2, btnY + (btnH - th) / 2, text, COLOR_WHITE, 18);

    clicked = hover && gl.IsMousePressed(MOUSE_LEFT);
}

static void HandleSounds(GameLib &gl, GameState prevState, GameState curState,
                         int prevMoveCount, int curMoveCount,
                         bool wasAnimating, bool isAnimating,
                         bool buttonClicked)
{
    if (curMoveCount > prevMoveCount && curState == STATE_PLAYING &&
        !isAnimating && !wasAnimating)
    {
        gl.PlayBeep(660, 60, 1, 600);
    }

    if (!wasAnimating && isAnimating)
    {
        gl.PlayBeep(330, 80, 1, 600);
    }

    if (curState != prevState)
    {
        if (curState == STATE_X_WIN)
        {
            gl.PlayBeep(800, 120, 1, 800);
            gl.PlayBeep(1000, 180, 1, 800);
        }
        else if (curState == STATE_O_WIN)
        {
            gl.PlayBeep(600, 120, 1, 800);
            gl.PlayBeep(400, 180, 1, 800);
        }
        else if (curState == STATE_DRAW)
        {
            gl.PlayBeep(500, 100, 1, 700);
            gl.PlayBeep(350, 100, 1, 700);
        }
    }

    if (buttonClicked)
    {
        gl.PlayBeep(500, 30, 1, 500);
    }
}

int main()
{
    GameLib gl;
    gl.Open(WIN_W, WIN_H, "Tic Tac Toe", true, true);
    gl.AspectLock(true, COLOR_WHITE);

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

    int xSprite = CreateXSprite(gl);

    gl.ShowFps(true);

    int splashPhase = 0;
    int splashTimer = 0;

    GameState prevState = STATE_PLAYING;
    int prevMoveCount = 0;
    bool wasAnimating = false;

    while (!gl.IsClosed())
    {
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

        if (gl.IsKeyPressed(KEY_R))
            game.Reset();

        if (gl.IsMousePressed(MOUSE_LEFT))
            game.HandleClick(gl.GetMouseX(), gl.GetMouseY(),
                             GRID_X, GRID_Y, CELL);

        int curMoveCount = game.GetBoard().GetMoveCount();
        bool isAnimating = game.IsAIMoveAnimating();

        game.Update();

        GameState state = game.GetState();
        gl.Clear(COLOR_WHITE);

        DrawGrid(gl);
        DrawBoard(gl, game.GetBoard(), xSprite, star);

        if (!isAnimating)
            DrawWinLine(gl, game.GetWinLine());

        DrawHoverPreview(gl, game, xSprite, star);

        if (kirby >= 0)
        {
            int frameIndex = (int)game.GetAISpriteAnim() * 5 + game.GetAISpriteFrame();
            int spriteX = GRID_X + BOARD_SIZE * CELL + 30;
            int spriteY = GRID_Y;
            gl.DrawSpriteFrameScaled(kirby, spriteX, spriteY,
                                     kirbyFW, kirbyFH, frameIndex,
                                     KIRBY_DW, KIRBY_DH, SPRITE_ALPHA);
        }

        DrawAIFlyingPiece(gl, game, star);

        bool buttonClicked = false;
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

            DrawPlayAgainButton(gl, buttonClicked);
            if (buttonClicked)
                game.Reset();
        }

        gl.DrawTextFont(10, WIN_H - 22, "\xe6\x8c\x89 R \xe9\x87\x8d\xe6\x96\xb0\xe5\xbc\x80\xe5\xa7\x8b", COLOR_DARK_GRAY, 14);

        HandleSounds(gl, prevState, state, prevMoveCount, curMoveCount,
                     wasAnimating, isAnimating, buttonClicked);

        prevState = state;
        prevMoveCount = curMoveCount;
        wasAnimating = isAnimating;

        gl.Update();
        gl.WaitFrame(60);
    }

    if (kirby >= 0) gl.FreeSprite(kirby);
    if (splash >= 0) gl.FreeSprite(splash);
    if (star >= 0) gl.FreeSprite(star);
    if (xSprite >= 0) gl.FreeSprite(xSprite);
    return 0;
}

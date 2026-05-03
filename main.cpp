/**
 * main.cpp —— 井字棋游戏主入口
 *
 * 功能：
 *   - 初始化窗口与游戏引擎
 *   - 处理键盘/鼠标输入
 *   - 渲染棋盘、棋子及游戏状态信息
 *   - 驱动游戏主循环
 *
 * 图形引擎：GameLib（单头文件 Win32 GDI 游戏库）
 */

#define GAMELIB_IMPLEMENTATION
#include "lib/GameLib.h"
#include "src/Game.h"

/** 窗口宽度 */
const int WIN_W = 640;
/** 窗口高度 */
const int WIN_H = 480;
/** 每个格子的像素大小 */
const int CELL = 100;
/** 棋盘左上角 X 坐标（居中计算） */
const int GRID_X = (WIN_W - CELL * BOARD_SIZE) / 2;
/** 棋盘左上角 Y 坐标（居中后向上偏移 20 像素） */
const int GRID_Y = (WIN_H - CELL * BOARD_SIZE) / 2 - 20;

/**
 * 绘制棋盘网格线
 * @param gl  GameLib 图形引擎引用
 */
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

/**
 * 在指定格子绘制棋子符号（X 或 O）
 * @param gl   GameLib 图形引擎引用
 * @param row  棋子所在行
 * @param col  棋子所在列
 * @param cell 棋子类型（CELL_X 画蓝色 X，CELL_O 画金色 O）
 */
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

/**
 * 遍历棋盘，绘制所有非空格子中的棋子
 * @param gl    GameLib 图形引擎引用
 * @param board 棋盘数据
 */
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

/**
 * 程序入口
 *   - 创建 640x480 窗口，标题 "Tic Tac Toe"，居中显示
 *   - 进入主循环，处理输入、更新游戏状态、渲染画面
 *   - 按键 1/2 切换 PvP/PvE 模式，R 键重置游戏
 *   - 固定 60 FPS 帧率
 */
int main()
{
    GameLib gl;
    gl.Open(WIN_W, WIN_H, "Tic Tac Toe", true);

    Game game;

    gl.ShowFps(true);

    while (!gl.IsClosed())
    {
        /* --- 键盘输入处理 --- */
        if (gl.IsKeyPressed(KEY_1))
            game.SetMode(MODE_PVP);
        if (gl.IsKeyPressed(KEY_2))
            game.SetMode(MODE_PVE);
        if (gl.IsKeyPressed(KEY_R))
            game.Reset(game.GetMode());

        /* --- 鼠标输入处理 --- */
        if (gl.IsMousePressed(MOUSE_LEFT))
            game.HandleClick(gl.GetMouseX(), gl.GetMouseY(),
                             GRID_X, GRID_Y, CELL);

        /* --- 更新游戏逻辑（AI 走棋等） --- */
        game.Update();

        /* --- 渲染 --- */
        gl.Clear(COLOR_BLACK);
        DrawGrid(gl);
        DrawBoard(gl, game.GetBoard());

        /* --- 显示模式提示文字 --- */
        const char *modeHint = (game.GetMode() == MODE_PVP)
                                   ? "1: PvP  |  2: PvE"
                                   : "1: PvP  |  2: PvE *";
        gl.DrawText(10, 6, modeHint, COLOR_DARK_GRAY);

        /* --- 显示游戏状态信息 --- */
        GameState state = game.GetState();
        if (state == STATE_PLAYING)
        {
            Cell cur = game.GetCurrentPlayer();
            const char *name = (cur == CELL_X) ? "X" : "O";
            uint32_t col = (cur == CELL_X) ? COLOR_SKY_BLUE : COLOR_GOLD;
            gl.DrawText(10, 22, "Turn: ", COLOR_WHITE);
            gl.DrawText(58, 22, name, col);

            if (game.GetMode() == MODE_PVE && cur == CELL_O && game.GetAIDelay() > 0)
                gl.DrawText(80, 22, "- AI thinking...", COLOR_GRAY);
            else
                gl.DrawText(80, 22, "- click a cell", COLOR_GRAY);
        }
        else
        {
            /* --- 游戏结束：显示胜负/平局结果 --- */
            const char *msg;
            uint32_t col;
            if (state == STATE_X_WIN)
            {
                msg = "X wins!";
                col = COLOR_SKY_BLUE;
            }
            else if (state == STATE_O_WIN)
            {
                msg = "O wins!";
                col = COLOR_GOLD;
            }
            else
            {
                msg = "Draw!";
                col = COLOR_GRAY;
            }

            int tw = 8 * (int)strlen(msg);
            gl.DrawText((WIN_W - tw) / 2, GRID_Y + BOARD_SIZE * CELL + 20, msg, col);
        }

        gl.DrawText(10, WIN_H - 20, "R = restart", COLOR_DARK_GRAY);
        gl.Update();
        gl.WaitFrame(60);
    }
    return 0;
}
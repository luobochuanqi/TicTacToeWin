/**
 * Player.cpp —— 玩家基类的实现
 */

#include "Player.h"

Player::Player(Cell symbol) : symbol(symbol) {}

Cell Player::GetSymbol() const
{
    return symbol;
}
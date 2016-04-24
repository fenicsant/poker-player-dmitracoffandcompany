#include "player.h"

const char* Player::VERSION = "Default C++ folding player";

int Player::betRequest(json::Value game_state)
{
    int bet = random();
    return bet;
}

void Player::showdown(json::Value game_state)
{
}

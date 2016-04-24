#include "player.h"

#include <list>


const char* Player::VERSION = "Default C++ folding player";

class Hand;

/*
using namespace std;

class Card
{
public:
    enum Suit { hearts, spades};
    Suit suit;
    int rank; // 2-2 .. 10-10 11-V 12-D 13-K 14-A
};

class Table
{
public :
    Hand * self;
    list<Hand*> player;
    list<Card> common;
};

class Hand
{
   list<Card> cards;
   int cost();
};

*/

int Player::betRequest(json::Value game_state)
{
    int bet = 150;
    return bet;
}

void Player::showdown(json::Value game_state)
{
}

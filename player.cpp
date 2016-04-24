#include "player.h"

#include <list>
#include <iostream>


const char* Player::VERSION = "Default C++ folding player";

class Hand;


using namespace std;

class Card
{
public:
    enum Suit { hearts, spades, clubs, diamonds};
    Suit suit;
    int rank; // 2-2 .. 10-10 11-V 12-D 13-K 14-A
    Card():suit(hearts),rank(0){}
    Card(Suit s, int r):suit(s),rank(r){}
    Card(const string & s,const string &r);
    iostream operator<<(const iostream &b);
};

class Table
{
public :
    Hand * self;
    std::list<int> bet;
    std::list<Card> common;
    Table();//:self(new Hand()){}
};

class Hand
{
public:
   std::list<Card> cards;
   int cost();
};


Table::Table():self(new Hand()){}

#define BadJsonBet 150


int Player::betRequest(json::Value game_state)
{
    if (game_state.GetType()!=json::ObjectVal) {
        return BadJsonBet;
    }
    json::Object gsObj = game_state.ToObject();

    json::Value players = gsObj["players"];
    if (players.GetType() != json::ArrayVal) {
        return BadJsonBet;
    }
    Table table;
    json::Array aplayers = players.ToArray();
    for (int i=aplayers.size()-1; i>=0; --i) {
        if (aplayers[i].GetType() != json::ObjectVal) continue;
        json::Object player = aplayers[i].ToObject();
        if (player["name"].ToString() == "DmitracoffAndCompany") {
            json::Array cards = player["hole_cards"].ToArray();
            if (cards.size()<2) return 0;
            table.self->cards.push_back(Card(cards[0]["suit"].ToString(),cards[0]["rank"].ToString()));
            table.self->cards.push_back(Card(cards[1]["suit"].ToString(),cards[1]["rank"].ToString()));
            Card c1 = table.self->cards.front();
            Card c2 = table.self->cards.back();
            cerr<<"cards: "<<c1.suit<<c1.rank<<c2.suit<<c2.rank<<endl;
        }
        cerr<<"player:"<<player["name"].ToString()<<endl;
    }


    cerr<<"ok"<<endl;

    Card c1 = table.self->cards.front();
    Card c2 = table.self->cards.back();
    if (c1.rank == c2.rank)
        return 1000;



    int bet = 150;
    return bet;
}

void Player::showdown(json::Value game_state)
{
}


Card::Card(const string &s, const string &r)
    :suit(hearts),rank(0)
{ //hearts, spades, clubs, diamonds
    if (s=="spades") suit=spades;
    if (s=="clubs") suit=clubs;
    if (s=="diamonds") suit=diamonds;

    if (r=="A") rank = 14;
    if (r=="K") rank = 13;
    if (r=="Q") rank = 12;
    if (r=="J") rank = 11;
    if (r=="10") rank = 10;
    if (rank==0 && r.size()>0) rank=r[0] - '0';
}

iostream Card::operator<<(const iostream &b)
{
    //return (b<<"["<<rank<<suit<<"]");
}

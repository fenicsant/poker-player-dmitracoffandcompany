#include "player.h"

#include <list>
#include <vector>
#include <iostream>
#include <algorithm>


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
   std::vector<Card> cards;
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

    int cur_bet = 0;
    int SelfStack = 0;
    int playerCount = aplayers.size();


    for (int i=aplayers.size()-1; i>=0; --i) {
        if (aplayers[i].GetType() != json::ObjectVal) continue;
        json::Object player = aplayers[i].ToObject();
        if (player["name"].ToString() == "DmitracoffAndCompany") {
            json::Array cards = player["hole_cards"].ToArray();
            if (cards.size()<2) return 0;
            table.self->cards.push_back(Card(cards[0]["suit"].ToString(),cards[0]["rank"].ToString()));
            table.self->cards.push_back(Card(cards[1]["suit"].ToString(),cards[1]["rank"].ToString()));
            SelfStack = player["stack"].ToInt();
        } else {
            int bet=player["bet"].ToInt();
            if (cur_bet<bet) cur_bet = bet;
        }
    }

    json::Array jsComm = gsObj["community_cards"].ToArray();
    vector<Card> comm(jsComm.size());
    for(int  i=jsComm.size()-1; i>=0; --i) {
        comm[i]=Card(jsComm[i]["suit"].ToString(),jsComm[i]["rank"].ToString());
    }


    cerr<<"ok"<<endl;

    Card c1 = table.self->cards.front();
    Card c2 = table.self->cards.back();

    if (comm.size()>1) {
        //proverka na set i kare
        cerr << "stack =" << SelfStack << endl;
        if (c1.rank == c2.rank) {
            int count = 2;
            for (int i=0; i < comm.size(); i++)
                if (comm[i].rank == c1.rank)
                    count++;
            if (count > 2)
                return SelfStack;
        } else {
            int count1 = 1;
            for (int i=0; i < comm.size(); i++)
                if (comm[i].rank == c1.rank)
                    count1++;
            int count2 = 1;
            for (int i=0; i < comm.size(); i++)
                if (comm[i].rank == c2.rank)
                    count2++;
            if (count1 + count2 > 3)
                return SelfStack;
        }
        //proverka flash
        if (c1.suit == c2.suit)
        {
            int count = 2;
            for (int i=0; i < comm.size(); i++)
                if (comm[i].suit == c1.suit)
                    count++;
            if (comm.size() == 3)
            {
                if (count == 5) {
                    return SelfStack;
                }
                if (count == 4) {
                    return cur_bet > SelfStack/ 5 ? cur_bet: SelfStack/ 5;
                }
            }
            if (comm.size() == 4)
            {
                if (count == 5) {
                    return SelfStack;
                }
                if (count == 4) {
                    return cur_bet > SelfStack/ 5 ? cur_bet: SelfStack/ 5;
                }
            }
            if (comm.size() == 5)
            {
                if (count == 5) {
                    return SelfStack;
                }
            }
        } else {
            int count1 =1;
            for (int i=0; i < comm.size(); i++)
                if (comm[i].suit == c1.suit)
                    count1++;
            int count2 = 1;
            for (int i=0; i < comm.size(); i++)
                if (comm[i].suit == c2.suit)
                    count2++;
            int count = count1 >= count2 ? count1 : count2;
            if (comm.size() == 4)
            {
                if (count == 5) {
                    return SelfStack;
                }
                if (count == 4) {
                    return cur_bet > SelfStack/ 5 ? cur_bet: SelfStack/ 5;
                }
            }
            if (comm.size() == 5)
            {
                if (count == 5) {
                    return SelfStack;
                }
            }
        }
    }


    if (abs(c1.rank - c2.rank) > 3  && (c1.suit!=c2.suit) && playerCount>3 && !(c1.rank == 14 || (c2.rank == 14)))
        return 0;

    if ((c1.rank == c2.rank +1 || c1.rank == c2.rank-1) && c1.suit == c2.suit)
        return 500;
    if (c1.rank == c2.rank)
        return 300;
    if (c1.suit == c2.suit)
        return 200;

    if (c1.rank == c2.rank +1 || c1.rank == c2.rank-1)
        return 200;

    return (cur_bet>200)?0:cur_bet;
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


int Hand::cost()
{
    bool street = true;
    bool flash = true;
    vector<int> ranks(cards.size());
    for(int i=cards.size(); i>=0; --i) {
        ranks[i] = cards[i].rank;
        if (i>0 && cards[i].suit != cards[i-1].suit) flash = false;
    }
    sort(ranks.begin(),ranks.end());
    int dif = 4 - cards.size();
    for(int i=ranks.size(); i>=1 && street; --i) {
        if (ranks[i]-ranks[i-1]>dif) {
            street = false;
            break;
        }
        if (ranks[i]-ranks[i-1]==1) {
            continue;
        }
        dif -= ranks[i] - ranks[i-1] +1;
    }

    if (cards.size()==2) {
        if (cards[0].rank == cards[1].rank) return 50;
        if (cards[0].suit == cards[1].suit) return 30;
        if (cards[0].rank > 10 && cards[1].rank > 10) return 20;
        if (abs(cards[0].rank-cards[1].rank) == 1) return 10;
        return 0;
    }
    int sfcost = 0;
    if ( flash || street ) {
        sfcost = (street)?cards.size()*20:0 + (flash)?cards.size()*20:0;
        if (sfcost>=100) return 100;
    }
    if (cards.size()==3) {
        if (cards[0].rank == cards[1].rank == cards[2].rank ) return 100;
    }
    if (cards.size() == 4) {
        if (cards[0].rank==cards[1].rank == cards[2].rank == cards[3].rank) return 100;
    }
    return 0;
}

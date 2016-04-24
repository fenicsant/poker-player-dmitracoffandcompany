#include<iostream>
#include<string>
#include "json.h"
#include "player.h"


using namespace std;

int main(int argc, char* argv[])
{
    string action = (argc>1)?argv[1]:"";
    string game_state_json;
    getline(cin, game_state_json);
    cout<<"cout:"<<endl<<game_state_json<<endl;

    json::Value game_state = json::Deserialize(game_state_json);

    if(action == "bet_request")
    {
        cout << Player::betRequest(game_state);
    }
    else if(action == "showdown")
    {
        Player::showdown(game_state);
    }
    else if(action == "version")
    {
        cout << Player::VERSION;
    }

    return 0;
}

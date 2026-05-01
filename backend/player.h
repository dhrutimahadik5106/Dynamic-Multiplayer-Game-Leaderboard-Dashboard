#pragma once
#include <string>
using namespace std;

struct Player
{
    int id;
    string username;
    int score;

    Player()
    {
        id = 0;
        username = "";
        score = 0;
    }
};

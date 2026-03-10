#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct Player
{
    int id;
    string username;
    int score;
};

class Leaderboard
{
private:

    vector<Player> players;

    unordered_map<int,int> playerIndex;

    unordered_map<int, Player> playerMap;

    vector<string> activityLog;

    int nextId;

public:

    Leaderboard();

    Player addPlayer(string username, int score);

    void updateScore(int id,int change);

    Player getPlayerById(int id);

    vector<Player> getLeaderboard();

    vector<Player> getTopPlayers(int n);

    vector<string> getActivity();

    int totalPlayers();

    int topScore();

    double avgScore();

    void simulateRandomUpdate();
};

#endif

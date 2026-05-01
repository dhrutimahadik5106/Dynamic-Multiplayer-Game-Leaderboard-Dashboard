#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <vector>
#include <string>
#include <unordered_map>
#include "player.h"
#include "avl_tree.h"
#include "heap.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class Leaderboard
{
private:

    vector<Player> players;

    unordered_map<int,int> playerIndex;

    unordered_map<int, Player> playerMap;

    vector<string> activityLog;

    int nextId;

    AVLTree avlTree;
    MaxHeap maxHeap;


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

    json getDataStructureStats();

    json getNetworkData();
};

#endif
#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include "player.h"
#include "activity_list.h"
#include "avl_tree.h"
#include "heap.h"

class Leaderboard
{
private:
    Player players[500];
    int playerCount;
    ActivityList activityList;
    AVLTree avlTree;
    MaxHeap maxHeap;
    int nextId;

public:
    Leaderboard();
    Player addPlayer(char* username, int score);
    void updateScore(int id, int change);
    Player getPlayerById(int id);
    void getLeaderboard(Player* result, int* count);
    void getTopPlayers(int n, Player* result, int* count);
    void getActivity(char activities[][200], int* count);
    int getTotalPlayers();
    int getTopScore();
    double getAvgScore();
    void simulateRandomUpdate();
};

#endif
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

struct Player
{
    int id;
    char username[50];
    int score;
};

#include "activity_list.cpp"
#include "avl_tree.cpp"
#include "heap.cpp"

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

Leaderboard::Leaderboard()
{
    playerCount = 0;
    nextId = 1;
}

Player Leaderboard::addPlayer(char* username, int score)
{
    Player p;
    p.id = nextId;
    nextId++;
    
    int i = 0;
    while(username[i] != '\0' && i < 49)
    {
        p.username[i] = username[i];
        i++;
    }
    p.username[i] = '\0';
    p.score = score;
    
    if(playerCount < 500)
    {
        players[playerCount] = p;
        playerCount++;
    }
    
    avlTree.insert(p);
    maxHeap.insert(p);
    
    char msg[200];
    char* ptr = msg;
    
    int j = 0;
    char welcomeMsg[] = "Player ";
    while(welcomeMsg[j] != '\0')
    {
        *ptr = welcomeMsg[j];
        ptr++;
        j++;
    }
    
    j = 0;
    while(username[j] != '\0')
    {
        *ptr = username[j];
        ptr++;
        j++;
    }
    
    char joinedMsg[] = " joined the game!";
    j = 0;
    while(joinedMsg[j] != '\0')
    {
        *ptr = joinedMsg[j];
        ptr++;
        j++;
    }
    *ptr = '\0';
    
    activityList.addActivity(msg);
    
    return p;
}

void Leaderboard::updateScore(int id, int change)
{
    for(int i = 0; i < playerCount; i++)
    {
        if(players[i].id == id)
        {
            players[i].score += change;
            
            char msg[200];
            char* ptr = msg;
            
            int j = 0;
            char scoreMsg[] = "Player ";
            while(scoreMsg[j] != '\0')
            {
                *ptr = scoreMsg[j];
                ptr++;
                j++;
            }
            
            j = 0;
            while(players[i].username[j] != '\0')
            {
                *ptr = players[i].username[j];
                ptr++;
                j++;
            }
            
            char scoreMsgEnd[] = " score updated!";
            j = 0;
            while(scoreMsgEnd[j] != '\0')
            {
                *ptr = scoreMsgEnd[j];
                ptr++;
                j++;
            }
            *ptr = '\0';
            
            activityList.addActivity(msg);
            return;
        }
    }
}

Player Leaderboard::getPlayerById(int id)
{
    for(int i = 0; i < playerCount; i++)
    {
        if(players[i].id == id)
            return players[i];
    }
    
    Player empty;
    empty.id = -1;
    empty.username[0] = '\0';
    empty.score = 0;
    return empty;
}

void Leaderboard::getLeaderboard(Player* result, int* count)
{
    avlTree.getSortedPlayers(result, count);
}

void Leaderboard::getTopPlayers(int n, Player* result, int* count)
{
    maxHeap.getTopN(result, n, count);
}

void Leaderboard::getActivity(char activities[][200], int* count)
{
    activityList.getActivities(activities, count);
}

int Leaderboard::getTotalPlayers()
{
    return playerCount;
}

int Leaderboard::getTopScore()
{
    if(playerCount == 0)
        return 0;

    int maxScore = players[0].score;
    for(int i = 1; i < playerCount; i++)
    {
        if(players[i].score > maxScore)
            maxScore = players[i].score;
    }

    return maxScore;
}

double Leaderboard::getAvgScore()
{
    if(playerCount == 0)
        return 0;
    
    int total = 0;
    for(int i = 0; i < playerCount; i++)
        total += players[i].score;
    
    return (double)total / playerCount;
}

void Leaderboard::simulateRandomUpdate()
{
    if(playerCount > 0)
    {
        int idx = rand() % playerCount;
        int change = (rand() % 100) - 50;
        updateScore(players[idx].id, change);
    }
}

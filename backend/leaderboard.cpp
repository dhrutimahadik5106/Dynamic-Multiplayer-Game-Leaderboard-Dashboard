#include <iostream>
#include <cstring>
#include <cstdlib>
#include "player.h"
<<<<<<< HEAD
#include "json.hpp"
#include <algorithm>
#include <ctime>
#include <random>
#include <mutex>

using json = nlohmann::json;

// Thread-safe random number generation
extern std::random_device rd;
extern std::mt19937 gen;
extern std::mutex randMutex;

extern int getRandomInt(int min, int max);

// Declarations

struct Node
{
    Player player;
    Node* left;
    Node* right;
    int height;
    Node(Player p)
    {
        player = p;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};

class AVLTree
{
private:
    Node* root;
    int height(Node* node);
    int getBalance(Node* node);
    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);
    Node* insertNode(Node* node, Player player);
    Node* deleteNode(Node* root, Player player);
    Node* minValueNode(Node* node);
    void inorder(Node* node, vector<Player>& players);
public:
    AVLTree();
    void insert(Player player);
    void remove(Player player);
    vector<Player> getSortedPlayers();
    int getHeight();
};

class MaxHeap
{
private:
    vector<Player> heap;
    int parent(int i);
    int left(int i);
    int right(int i);
    void heapifyUp(int index);
    void heapifyDown(int index);
public:
    MaxHeap();
    void insert(Player player);
    Player extractMax();
    Player getMax();
    vector<Player> getTopN(int n);
    int size();
};

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
    int totalMatches;
    int recordsBroken;
    int previousTopScore;
    int activityEventsSinceLastStats;
    int joinEventsSinceLastStats;
    int matchEventsSinceLastStats;
    int scoreUpdateEventsSinceLastStats;
    vector<double> avgScoreHistory;
    vector<int> topScoreHistory;
    vector<int> totalPlayersHistory;
    vector<int> activityCountHistory;
    vector<int> joinHistory;
    vector<int> matchHistory;
    vector<int> scoreUpdateHistory;
    vector<string> timeLabels;
public:
    Leaderboard();
    Player addPlayer(string username, int score, bool trackRecord = true);
    void updateScore(int id,int change);
    void rebuildStructures();
    Player getPlayerById(int id);
    vector<Player> getLeaderboard();
    vector<Player> getTopPlayers(int n);
    vector<string> getActivity();
    int totalPlayers();
    int topScore();
    double avgScore();
    void simulateRandomUpdate();
    nlohmann::json getNetworkData();
    nlohmann::json getStatsData();
};
=======
#include "leaderboard.h"
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7

using namespace std;

Leaderboard::Leaderboard()
{
    playerCount = 0;
    nextId = 1;
<<<<<<< HEAD
    totalMatches = 0;
    recordsBroken = 0;
    previousTopScore = 0;
    activityEventsSinceLastStats = 0;
    joinEventsSinceLastStats = 0;
    matchEventsSinceLastStats = 0;
    scoreUpdateEventsSinceLastStats = 0;

    // Initialize history with some data
    for(int i = 0; i < 10; i++) {
        avgScoreHistory.push_back(0);
        topScoreHistory.push_back(0);
        totalPlayersHistory.push_back(0);
        activityCountHistory.push_back(0);
        joinHistory.push_back(0);
        matchHistory.push_back(0);
        scoreUpdateHistory.push_back(0);
        timeLabels.push_back("T-" + to_string(10-i));
    }

    vector<string> names = {
        "Shadow","Nova","Dragon","Phantom","Cyber","Dark","Ice","Storm","Blaze","Night",
        "Ghost","Thunder","Iron","Crimson","Vortex","Silver","Pixel","Quantum","Lunar","Turbo",
        "Frost","Neon","Sky","Alpha","Omega","Rapid","Steel","Mystic","Zero","Inferno",

        "Titan","Venom","Rogue","Falcon","Hunter","Sniper","Raptor","Wolf","Panther","Tiger",
        "Cobra","Eagle","Hawk","Lion","Rhino","Jaguar","Leopard","Berserker","Knight","Samurai",

        "Ninja","Warrior","Gladiator","Champion","Destroyer","Guardian","Assassin","Mercenary","Paladin","Warlord",
        "Commander","Captain","General","Soldier","Raider","Bandit","Outlaw","Viking","Spartan","Knightmare",

        "Fire","Flame","Ember","Blizzard","Cyclone","Tempest","Hurricane","Tornado","Avalanche","Quake",
        "Lightning","Meteor","Comet","Galaxy","Nebula","Cosmic","Solar","LunarX","Astro","Orbit",

        "Blade","Sword","Arrow","Hammer","Axe","Dagger","Claw","Fang","Spike","Edge"
    };

    for(int i = 0; i < 1000; i++)
    {
        string name = names[getRandomInt(0, names.size() - 1)] + to_string(getRandomInt(0, 99));
        int score = getRandomInt(0, 1999);

        addPlayer(name, score, false);
    }

    previousTopScore = topScore();

    // Update initial history
    avgScoreHistory.back() = avgScore();
    topScoreHistory.back() = topScore();
    totalPlayersHistory.back() = totalPlayers();
    activityCountHistory.back() = 0;
=======
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7
}

Player Leaderboard::addPlayer(char* username, int score)
{
    Player p;
<<<<<<< HEAD

    p.id = nextId++;
    p.username = username;
    p.score = score;

    players.push_back(p);

    // Hash table
    playerIndex[p.id] = players.size()-1;

    // Activity
    activityLog.insert(activityLog.begin(),
        username + " joined the leaderboard with " + to_string(score) + " points");
    if (activityLog.size() > 250)
        activityLog.resize(250);
    activityEventsSinceLastStats++;
    joinEventsSinceLastStats++;

    // Add to AVL Tree
    avlTree.insert(p);

    // Add to Max Heap
    maxHeap.insert(p);

    if(trackRecord)
=======
    p.id = nextId;
    nextId++;
    
    int i = 0;
    while(username[i] != '\0' && i < 49)
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7
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

        
            avlTree = AVLTree();
            maxHeap = MaxHeap();

            for(int j = 0; j < playerCount; j++)
            {
                avlTree.insert(players[j]);
                maxHeap.insert(players[j]);
            }

            
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

<<<<<<< HEAD
    string msg = players[index].username + " " + action;
    activityLog.insert(activityLog.begin(), msg);
    if (activityLog.size() > 250)
        activityLog.resize(250);
    activityEventsSinceLastStats++;
    scoreUpdateEventsSinceLastStats++;
=======
void Leaderboard::getLeaderboard(Player* result, int* count)
{
    avlTree.getSortedPlayers(result, count);
}
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7

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
<<<<<<< HEAD
    if(players.empty())
        return;

    int changes = 4 + getRandomInt(0, 2); // 4-6 players updated each cycle
    vector<int> updatedIds;

    for (int i = 0; i < changes; ++i)
    {
        int index = getRandomInt(0, players.size() - 1);
        int change = 0;
        if (getRandomInt(0, 1) == 0)
            change = 5 + getRandomInt(0, 55);   // gain +5 to +60
        else
            change = -(5 + getRandomInt(0, 35)); // loss -5 to -40

        players[index].score += change;
        if (players[index].score < 0)
            players[index].score = 0;

        string action;
        if (change > 0) {
            action = "gained " + to_string(change) + " points";
        } else {
            action = "lost " + to_string(abs(change)) + " points";
        }

        string msg = players[index].username + " " + action;
        activityLog.insert(activityLog.begin(), msg);
        if (activityLog.size() > 250)
            activityLog.resize(250);
        activityEventsSinceLastStats++;

        updatedIds.push_back(players[index].id);
    }

    rebuildStructures();

    totalMatches += 1;
    matchEventsSinceLastStats += 1;

    int currentTop = topScore();
    if (currentTop > previousTopScore) {
        recordsBroken++;
        previousTopScore = currentTop;
    }

    // Update history
    avgScoreHistory.push_back(avgScore());
    topScoreHistory.push_back(topScore());
    totalPlayersHistory.push_back(totalPlayers());
    activityCountHistory.push_back(activityEventsSinceLastStats);
    joinHistory.push_back(joinEventsSinceLastStats);
    matchHistory.push_back(matchEventsSinceLastStats);
    scoreUpdateHistory.push_back(scoreUpdateEventsSinceLastStats);

    // Generate time label
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", ltm);
    timeLabels.push_back(string(buffer));

    // Keep only last 20
    if (avgScoreHistory.size() > 20) {
        avgScoreHistory.erase(avgScoreHistory.begin());
        topScoreHistory.erase(topScoreHistory.begin());
        totalPlayersHistory.erase(totalPlayersHistory.begin());
        activityCountHistory.erase(activityCountHistory.begin());
        joinHistory.erase(joinHistory.begin());
        matchHistory.erase(matchHistory.begin());
        scoreUpdateHistory.erase(scoreUpdateHistory.begin());
        timeLabels.erase(timeLabels.begin());
    }

    activityEventsSinceLastStats = 0;
    joinEventsSinceLastStats = 0;
    matchEventsSinceLastStats = 0;
    scoreUpdateEventsSinceLastStats = 0;
}

// ==========================
// NETWORK DATA
// ==========================

json Leaderboard::getNetworkData()
{
    json result;

    // Nodes: top 50 players
    json nodes = json::array();
    auto topPlayers = getTopPlayers(50);
    int rank = 1;
    for (auto &p : topPlayers) {
        nodes.push_back({
            {"id", p.id},
            {"name", p.username},
            {"score", p.score},
            {"rank", rank++}
        });
    }
    result["nodes"] = nodes;

    // Links: simulate connections between players
    // For simplicity, connect players with similar ranks (close in leaderboard)
    json links = json::array();
    for (size_t i = 0; i < topPlayers.size(); ++i) {
        for (size_t j = i + 1; j < topPlayers.size() && j < i + 3; ++j) {  // connect to next 2
            int value = getRandomInt(1, 10);  // random value 1-10
            links.push_back({
                {"source", topPlayers[i].id},
                {"target", topPlayers[j].id},
                {"value", value}
            });
        }
    }
    result["links"] = links;

    return result;
}

// ==========================
// STATS DATA
// ==========================

json Leaderboard::getStatsData()
{
    json result;

    // Basic stats
    result["totalPlayers"] = totalPlayers();
    result["totalMatches"] = max(0, totalMatches);
    result["avgSessionTime"] = max(3.0, min(25.0, avgScore() / 10.0 + 4.0));
    result["recordsBroken"] = recordsBroken;

    // Top players
    json topPlayersJson = json::array();
    auto topPlayers = getTopPlayers(10);
    if (topPlayers.empty()) {
        vector<string> placeholderNames = {
            "ShadowFox", "NovaRift", "PixelFury", "BladeDrift", "CyberPulse",
            "VortexAce", "EchoStrike", "NeonViper", "QuantumJet", "TitanFlux"
        };
        for (int i = 0; i < 10; ++i) {
            topPlayersJson.push_back({
                {"username", placeholderNames[i]},
                {"score", 880 + i * 14}
            });
        }
    } else {
        for (auto &p : topPlayers) {
            topPlayersJson.push_back({
                {"username", p.username},
                {"score", p.score}
            });
        }
        while (topPlayersJson.size() < 10) {
            topPlayersJson.push_back({
                {"username", string("Player_") + to_string((int)topPlayersJson.size() + 1)},
                {"score", 800}
            });
        }
    }
    result["topPlayers"] = topPlayersJson;

    // Rank distribution
    auto allPlayers = getLeaderboard();
    int n = allPlayers.size();
    int top10Count = n > 0 ? max(1, (int)ceil(n * 0.1)) : 0;
    int top25Count = n > 0 ? max(1, (int)ceil(n * 0.25)) : 0;
    int top50Count = n > 0 ? max(1, (int)ceil(n * 0.5)) : 0;
    int bottom50Count = n > 0 ? max(0, n - top50Count) : 0;

    result["rankDistribution"] = {
        {"top10", top10Count},
        {"top25", max(0, top25Count - top10Count)},
        {"top50", max(0, top50Count - top25Count)},
        {"bottom50", bottom50Count}
    };

    // Trends and time labels
    vector<double> avg = avgScoreHistory;
    vector<double> top(topScoreHistory.begin(), topScoreHistory.end());
    vector<double> active(totalPlayersHistory.begin(), totalPlayersHistory.end());
    vector<string> labels = timeLabels;
    vector<int> activity = activityCountHistory;

    int desiredPoints = 10;
    int currentSize = (int)max({avg.size(), top.size(), active.size(), labels.size(), activity.size(), (size_t)desiredPoints});

    auto padDouble = [&](vector<double> &values, double fallback) {
        if (values.empty()) {
            values.assign(currentSize, fallback);
        }
        while ((int)values.size() < currentSize) {
            values.push_back(values.back());
        }
        if ((int)values.size() > currentSize) {
            values.erase(values.begin(), values.end() - currentSize);
        }
    };

    auto padString = [&](vector<string> &values) {
        if (values.empty()) {
            values.resize(currentSize);
        }
        while ((int)values.size() < currentSize) {
            values.insert(values.begin(), string("T-") + to_string(currentSize - values.size()));
        }
        if ((int)values.size() > currentSize) {
            values.erase(values.begin(), values.end() - currentSize);
        }
        for (int i = 0; i < (int)values.size(); ++i) {
            if (values[i].empty()) {
                values[i] = string("T-") + to_string((int)values.size() - i);
            }
        }
    };

    auto padInt = [&](vector<int> &values, int fallback) {
        if (values.empty()) {
            values.assign(currentSize, fallback);
        }
        while ((int)values.size() < currentSize) {
            values.push_back(values.back());
        }
        if ((int)values.size() > currentSize) {
            values.erase(values.begin(), values.end() - currentSize);
        }
    };

    padDouble(avg, avgScore());
    padDouble(top, topScore());
    padDouble(active, totalPlayers());
    padString(labels);
    padInt(activity, joinHistory.empty() ? 0 : activityCountHistory.back());

    result["trends"] = {
        {"avgScore", avg},
        {"topScore", top},
        {"activePlayers", active}
    };

    result["activityTimeline"] = activity;
    result["timeLabels"] = labels;

    return result;
=======
    if(playerCount > 0)
    {
        int idx = rand() % playerCount;
        int change = (rand() % 100) - 50;
        updateScore(players[idx].id, change);
    }
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7
}

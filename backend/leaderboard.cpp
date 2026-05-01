#include "leaderboard.h"
#include <algorithm>
#include <cstdlib>
#include "json.hpp"

#include "avl_tree.h"
#include "heap.h"

using json = nlohmann::json;

Leaderboard::Leaderboard()
{
    nextId = 1;

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
        string name = names[rand() % names.size()] + to_string(rand() % 100);
        int score = rand() % 2000;

        addPlayer(name, score);
    }
}


// ==========================
// ADD PLAYER
// ==========================

Player Leaderboard::addPlayer(string username,int score)
{
    Player p;

    p.id = nextId++;
    p.username = username;
    p.score = score;

    players.push_back(p);

    // Hash table
    playerIndex[p.id] = players.size()-1;

    // Activity
    activityLog.insert(activityLog.begin(),
        username + " joined with score " + to_string(score));

    // Add to AVL Tree
    avlTree.insert(p);

    // Add to Max Heap
    maxHeap.insert(p);

    return p;
}


// ==========================
// SEARCH PLAYER BY ID
// ==========================

Player Leaderboard::getPlayerById(int id)
{
    if(playerIndex.find(id) == playerIndex.end())
        return Player();

    int index = playerIndex[id];

    return players[index];
}


// ==========================
// UPDATE SCORE
// ==========================

void Leaderboard::updateScore(int id,int change)
{
    if(playerIndex.find(id)==playerIndex.end())
        return;

    int index = playerIndex[id];

    players[index].score += change;

    string msg = players[index].username +
        " score changed by " + to_string(change);

    activityLog.insert(activityLog.begin(),msg);
}


// ==========================
// GET LEADERBOARD
// ==========================

vector<Player> Leaderboard::getLeaderboard()
{
    vector<Player> copy = players;

    sort(copy.begin(),copy.end(),
        [](Player a,Player b)
        {
            return a.score > b.score;
        });

    return copy;
}


// ==========================
// GET TOP PLAYERS
// ==========================

vector<Player> Leaderboard::getTopPlayers(int n)
{
    vector<Player> sorted = getLeaderboard();

    if(sorted.size()>n)
        sorted.resize(n);

    return sorted;
}


// ==========================
// ACTIVITY FEED
// ==========================

vector<string> Leaderboard::getActivity()
{
    vector<string> copy = activityLog;

    if(copy.size()>10)
        copy.resize(10);

    return copy;
}


// ==========================
// STATS
// ==========================

int Leaderboard::totalPlayers()
{
    return players.size();
}

int Leaderboard::topScore()
{
    int maxScore = 0;

    for(auto &p:players)
        if(p.score > maxScore)
            maxScore = p.score;

    return maxScore;
}

double Leaderboard::avgScore()
{
    if(players.empty())
        return 0;

    int sum = 0;

    for(auto &p:players)
        sum += p.score;

    return (double)sum / players.size();
}


// ==========================
// RANDOM UPDATE
// ==========================

void Leaderboard::simulateRandomUpdate()
{
    if(players.empty())
        return;

    int index = rand()%players.size();

    int change = (rand()%100)-50;

    players[index].score += change;

    string msg = players[index].username +
        " random update " + to_string(change);

    activityLog.insert(activityLog.begin(),msg);
}

// ==========================
// DATA STRUCTURE STATS
// ==========================

json Leaderboard::getDataStructureStats()
{
    json stats = {
        {"totalPlayers", (int)players.size()},
        {"avlTreeHeight", avlTree.getHeight()},
        {"maxHeapSize", maxHeap.size()},
        {"hashTableSize", (int)playerIndex.size()},
        {"activityLogSize", (int)activityLog.size()}
    };

    return stats;
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
            int value = rand() % 10 + 1;  // random value 1-10
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

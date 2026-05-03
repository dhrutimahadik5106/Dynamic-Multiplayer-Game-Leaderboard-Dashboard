#include <vector>
#include <string>
#include <unordered_map>
#include "player.h"
#include "json.hpp"
#include <algorithm>

using json = nlohmann::json;

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
    vector<double> avgScoreHistory;
    vector<int> topScoreHistory;
    vector<int> totalPlayersHistory;
    vector<int> activityCountHistory;
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
    nlohmann::json getDataStructureStats();
    nlohmann::json getNetworkData();
    nlohmann::json getStatsData();
};

using namespace std;

/*-------------------------------------------------------
    Player Comparison Helpers
-------------------------------------------------------*/

bool isLess(Player a, Player b)
{
    if (a.score != b.score)
        return a.score < b.score;

    return a.id < b.id;
}

bool isGreater(Player a, Player b)
{
    if (a.score != b.score)
        return a.score > b.score;

    return a.id > b.id;
}

/*-------------------------------------------------------
    Constructor
-------------------------------------------------------*/

AVLTree::AVLTree()
{
    root = nullptr;
}

/*-------------------------------------------------------
    Get Height
-------------------------------------------------------*/

int AVLTree::height(Node* node)
{
    if (node == nullptr)
        return 0;

    return node->height;
}

/*-------------------------------------------------------
    Balance Factor
-------------------------------------------------------*/

int AVLTree::getBalance(Node* node)
{
    if (node == nullptr)
        return 0;

    return height(node->left) - height(node->right);
}

/*-------------------------------------------------------
    Right Rotation
-------------------------------------------------------*/

Node* AVLTree::rotateRight(Node* y)
{
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

/*-------------------------------------------------------
    Left Rotation
-------------------------------------------------------*/

Node* AVLTree::rotateLeft(Node* x)
{
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

/*-------------------------------------------------------
    Insert Player
-------------------------------------------------------*/

Node* AVLTree::insertNode(Node* node, Player player)
{
    if (node == nullptr)
        return new Node(player);

    if (isLess(player, node->player))
        node->left = insertNode(node->left, player);

    else if (isGreater(player, node->player))
        node->right = insertNode(node->right, player);

    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    // Left Left
    if (balance > 1 && isLess(player, node->left->player))
        return rotateRight(node);

    // Right Right
    if (balance < -1 && isGreater(player, node->right->player))
        return rotateLeft(node);

    // Left Right
    if (balance > 1 && isGreater(player, node->left->player))
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    // Right Left
    if (balance < -1 && isLess(player, node->right->player))
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}

/*-------------------------------------------------------
    Find Minimum Node
-------------------------------------------------------*/

Node* AVLTree::minValueNode(Node* node)
{
    Node* current = node;

    while (current->left != nullptr)
        current = current->left;

    return current;
}

/*-------------------------------------------------------
    Delete Player
-------------------------------------------------------*/

Node* AVLTree::deleteNode(Node* root, Player player)
{
    if (root == nullptr)
        return root;

    if (isLess(player, root->player))
        root->left = deleteNode(root->left, player);

    else if (isGreater(player, root->player))
        root->right = deleteNode(root->right, player);

    else
    {
        if ((root->left == nullptr) || (root->right == nullptr))
        {
            Node* temp = root->left ? root->left : root->right;

            if (temp == nullptr)
            {
                temp = root;
                root = nullptr;
            }
            else
                *root = *temp;

            delete temp;
        }
        else
        {
            Node* temp = minValueNode(root->right);

            root->player = temp->player;

            root->right = deleteNode(root->right, temp->player);
        }
    }

    if (root == nullptr)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    // Left Left
    if (balance > 1 && getBalance(root->left) >= 0)
        return rotateRight(root);

    // Left Right
    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // Right Right
    if (balance < -1 && getBalance(root->right) <= 0)
        return rotateLeft(root);

    // Right Left
    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

/*-------------------------------------------------------
    Public Insert
-------------------------------------------------------*/

void AVLTree::insert(Player player)
{
    root = insertNode(root, player);
}

/*-------------------------------------------------------
    Public Delete
-------------------------------------------------------*/

void AVLTree::remove(Player player)
{
    root = deleteNode(root, player);
}

/*-------------------------------------------------------
    Inorder Traversal (Leaderboard)
-------------------------------------------------------*/

void AVLTree::inorder(Node* node, vector<Player>& players)
{
    if (node == nullptr)
        return;

    inorder(node->right, players);

    players.push_back(node->player);

    inorder(node->left, players);
}

/*-------------------------------------------------------
    Get Sorted Players
-------------------------------------------------------*/

vector<Player> AVLTree::getSortedPlayers()
{
    vector<Player> players;

    inorder(root, players);

    return players;
}

/*-------------------------------------------------------
    Get Tree Height
-------------------------------------------------------*/

int AVLTree::getHeight()
{
    return height(root);
}

/*-------------------------------------------------------
   Constructor
-------------------------------------------------------*/

MaxHeap::MaxHeap()
{
}

/*-------------------------------------------------------
   Get Parent Index
-------------------------------------------------------*/

int MaxHeap::parent(int i)
{
    return (i - 1) / 2;
}

/*-------------------------------------------------------
   Left Child Index
-------------------------------------------------------*/

int MaxHeap::left(int i)
{
    return (2 * i) + 1;
}

/*-------------------------------------------------------
   Right Child Index
-------------------------------------------------------*/

int MaxHeap::right(int i)
{
    return (2 * i) + 2;
}

/*-------------------------------------------------------
   Heapify Up
-------------------------------------------------------*/

void MaxHeap::heapifyUp(int index)
{
    while (index > 0 && heap[parent(index)].score < heap[index].score)
    {
        swap(heap[parent(index)], heap[index]);
        index = parent(index);
    }
}

/*-------------------------------------------------------
   Heapify Down
-------------------------------------------------------*/

void MaxHeap::heapifyDown(int index)
{
    int largest = index;
    int l = left(index);
    int r = right(index);

    if (l < heap.size() && heap[l].score > heap[largest].score)
        largest = l;

    if (r < heap.size() && heap[r].score > heap[largest].score)
        largest = r;

    if (largest != index)
    {
        swap(heap[index], heap[largest]);
        heapifyDown(largest);
    }
}

/*-------------------------------------------------------
   Insert Player into Heap
-------------------------------------------------------*/

void MaxHeap::insert(Player player)
{
    heap.push_back(player);

    int index = heap.size() - 1;

    heapifyUp(index);
}

/*-------------------------------------------------------
   Remove Top Player
-------------------------------------------------------*/

Player MaxHeap::extractMax()
{
    if (heap.empty())
        return Player();

    Player root = heap[0];

    heap[0] = heap.back();
    heap.pop_back();

    heapifyDown(0);

    return root;
}

/*-------------------------------------------------------
   Get Top Player
-------------------------------------------------------*/

Player MaxHeap::getMax()
{
    if (heap.empty())
        return Player();

    return heap[0];
}

/*-------------------------------------------------------
   Get Top N Players
-------------------------------------------------------*/

vector<Player> MaxHeap::getTopN(int n)
{
    vector<Player> result;

    vector<Player> temp = heap;

    sort(temp.begin(), temp.end(), [](Player a, Player b)
         {
             return a.score > b.score;
         });

    for (int i = 0; i < n && i < temp.size(); i++)
    {
        result.push_back(temp[i]);
    }

    return result;
}

/*-------------------------------------------------------
   Heap Size
-------------------------------------------------------*/

int MaxHeap::size()
{
    return heap.size();
}

Leaderboard::Leaderboard()
{
    nextId = 1;
    totalMatches = 0;
    recordsBroken = 0;
    previousTopScore = 0;
    activityEventsSinceLastStats = 0;

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

        addPlayer(name, score, false);
    }

    previousTopScore = topScore();
}

void Leaderboard::rebuildStructures()
{
    avlTree = AVLTree();
    maxHeap = MaxHeap();

    for (const auto& p : players)
    {
        avlTree.insert(p);
        maxHeap.insert(p);
    }
}


// ==========================
// ADD PLAYER
// ==========================

Player Leaderboard::addPlayer(string username,int score, bool trackRecord)
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
        username + " joined the leaderboard with " + to_string(score) + " points");
    if (activityLog.size() > 250)
        activityLog.resize(250);
    activityEventsSinceLastStats++;

    // Add to AVL Tree
    avlTree.insert(p);

    // Add to Max Heap
    maxHeap.insert(p);

    if(trackRecord)
    {
        bool inTop10 = false;
        auto topPlayers = getTopPlayers(10);
        for(const auto& player : topPlayers)
        {
            if(player.id == p.id)
            {
                inTop10 = true;
                break;
            }
        }

        if(p.score > previousTopScore)
        {
            recordsBroken++;
            previousTopScore = p.score;
        }
        else if(inTop10)
        {
            recordsBroken++;
        }
    }

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
    Player original = players[index];

    avlTree.remove(original);

    players[index].score += change;
    if(players[index].score < 0)
        players[index].score = 0;

    avlTree.insert(players[index]);
    rebuildStructures();

    string action;
    if (change > 0) {
        action = "gained " + to_string(change) + " points";
    } else if (change < 0) {
        action = "lost " + to_string(abs(change)) + " points";
    } else {
        action = "score unchanged";
    }

    string msg = players[index].username + " " + action;
    activityLog.insert(activityLog.begin(), msg);
    if (activityLog.size() > 250)
        activityLog.resize(250);
    activityEventsSinceLastStats++;

    int currentTop = topScore();
    if (currentTop > previousTopScore) {
        recordsBroken++;
        previousTopScore = currentTop;
    }
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

    if(copy.size() > 50)
        copy.resize(50);

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

    int changes = 4 + rand() % 3; // 4-6 players updated each cycle
    vector<int> updatedIds;

    for (int i = 0; i < changes; ++i)
    {
        int index = rand() % players.size();
        int change = 0;
        if (rand() % 2 == 0)
            change = 5 + rand() % 56;   // gain +5 to +60
        else
            change = -(5 + rand() % 36); // loss -5 to -40

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

    int currentTop = topScore();
    if (currentTop > previousTopScore) {
        recordsBroken++;
        previousTopScore = currentTop;
    }
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

// ==========================
// STATS DATA
// ==========================

json Leaderboard::getStatsData()
{
    json result;

    // Basic stats
    result["totalPlayers"] = totalPlayers();
    result["totalMatches"] = totalMatches;
    result["avgSession"] = max(3.0, min(25.0, avgScore() / 10.0 + 4.0));
    result["recordsBroken"] = recordsBroken;

    // Top 10 players
    json topPlayersJson = json::array();
    auto topPlayers = getTopPlayers(10);
    for (auto &p : topPlayers) {
        topPlayersJson.push_back({
            {"id", p.id},
            {"username", p.username},
            {"score", p.score}
        });
    }
    result["topPlayers"] = topPlayersJson;

    // Current values for trends
    result["avgScore"] = avgScore();
    result["topScore"] = topScore();

    // Rank distribution segments
    auto allPlayers = getLeaderboard();
    if (!allPlayers.empty()) {
        int n = allPlayers.size();
        int top10Count = max(1, (int)ceil(n * 0.1));
        int top25Count = max(1, (int)ceil(n * 0.25));
        int top50Count = max(1, (int)ceil(n * 0.5));
        int bottom50Count = max(0, n - top50Count);

        result["distribution"] = {
            {"top10", top10Count},
            {"top25", top25Count - top10Count},
            {"top50", top50Count - top25Count},
            {"bottom50", bottom50Count}
        };
    } else {
        result["distribution"] = {
            {"top10", 0},
            {"top25", 0},
            {"top50", 0},
            {"bottom50", 0}
        };
    }

    // Current raw activity count
    result["timeline"] = getActivity().size();
    result["dbPlayers"] = totalPlayers();
    result["dbMatches"] = totalMatches;
    result["dbActivity"] = getActivity().size();
    result["dbSize"] = max(5.0, (double)players.size() * 0.01 + 2.0);

    activityEventsSinceLastStats = 0;

    return result;
}

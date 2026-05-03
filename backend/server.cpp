#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>

#include "httplib.h"
#include "json.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "player.h"

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
public:
    Leaderboard();
    Player addPlayer(string username, int score, bool trackRecord = true);
    void updateScore(int id,int change);
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
using json = nlohmann::json;

Leaderboard leaderboard;
mutex dataMutex;


// ==========================
// JSON conversion
// ==========================
json playersToJson(vector<Player> players)
{
    json arr = json::array();
    int rank = 1;

    for(auto &p : players)
    {
        arr.push_back({
            {"rank", rank++},
            {"id", p.id},
            {"username", p.username},
            {"score", p.score}
        });
    }

    return arr;
}

string classifyActivityType(const string &activity)
{
    string lower = activity;
    for (auto &c : lower) c = tolower(c);

    if (lower.find("joined") != string::npos || lower.find("welcome") != string::npos)
        return "player";

    if (lower.find("defeated") != string::npos || lower.find("won") != string::npos || lower.find("completed") != string::npos || lower.find("battle") != string::npos || lower.find("ranked") != string::npos || lower.find("crushed") != string::npos || lower.find("beat") != string::npos || lower.find("dominated") != string::npos || lower.find("outplayed") != string::npos)
        return "match";

    if (lower.find("gained") != string::npos || lower.find("lost") != string::npos || lower.find("score changed") != string::npos || lower.find("score unchanged") != string::npos || lower.find("points") != string::npos || lower.find("+" ) != string::npos || lower.find("-") != string::npos)
        return "score";

    return "score";
}

json activityToJson(vector<string> activities)
{
    json result;
    json items = json::array();

    for(auto &a : activities)
    {
        items.push_back({
            {"type", classifyActivityType(a)},
            {"text", a}
        });
    }

    result["events"] = items;
    result["totalEvents"] = activities.size();
    return result;
}

string generateRandomUsername()
{
    vector<string> baseNames = {
        "Shadow","Nova","Dragon","Phantom","Cyber","Dark","Ice","Storm","Blaze","Night",
        "Ghost","Thunder","Vortex","Silver","Pixel","Quantum","Lunar","Turbo","Frost","Neon",
        "Sky","Alpha","Omega","Rapid","Steel","Mystic","Zero","Inferno","Rogue","Falcon"
    };

    vector<string> suffixes = {
        "X","Hunter","King","Storm","Blade","Ghost","Rider","Fury","Edge","Strike",
        "Knight","Star","Wing","Claw","Viper","Frost","Fire","Dragon","Ninja","Shadow"
    };

    string name = baseNames[rand() % baseNames.size()];
    string suffix = suffixes[rand() % suffixes.size()];
    int number = rand() % 90 + 10;

    return name + suffix + to_string(number);
}


int main()
{
    srand(time(0));

    cout << "Server starting...\n";

    httplib::Server server;

    server.Options(".*", [](const httplib::Request&, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 200;
    });

    thread liveUpdateThread([](){
        while(true)
        {
            this_thread::sleep_for(chrono::seconds(10));

            lock_guard<mutex> lock(dataMutex);

            int joins = 3 + rand() % 2; // 3-4 new players
            for (int i = 0; i < joins; ++i) {
                string username = generateRandomUsername();
                int score = 100 + rand() % 2101; // 100-2200
                leaderboard.addPlayer(username, score);
            }

            leaderboard.simulateRandomUpdate();
        }
    });
    liveUpdateThread.detach();

    // ================= API =================

    server.Get("/leaderboard", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);
        res.set_content(playersToJson(leaderboard.getLeaderboard()).dump(), "application/json");
    });

    server.Get("/topPlayers", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);
        res.set_content(playersToJson(leaderboard.getTopPlayers(5)).dump(), "application/json");
    });

    server.Get("/activity", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);
        res.set_content(activityToJson(leaderboard.getActivity()).dump(), "application/json");
    });

    server.Get("/stats", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);

        auto players = leaderboard.getLeaderboard();

        json stats = {
            {"totalPlayers", (int)players.size()},
            {"topScore", players.empty() ? 0 : players[0].score},
            {"avgScore", leaderboard.avgScore()},
            {"activeMatches", rand() % 81 + 20}
        };

        res.set_content(stats.dump(), "application/json");
    });

    server.Get("/api/stats", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);
        res.set_content(leaderboard.getStatsData().dump(), "application/json");
    });

    server.Get("/statsData", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);
        res.set_content(leaderboard.getStatsData().dump(), "application/json");
    });

    server.Get("/api/network", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);
        res.set_content(leaderboard.getNetworkData().dump(), "application/json");
    });

    server.Get("/networkStats", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);

        auto networkData = leaderboard.getNetworkData();
        auto nodes = networkData["nodes"];
        auto links = networkData["links"];

        int totalConnections = links.size();

        // Find node IDs that have connections
        unordered_set<int> connectedNodeIds;
        for (auto& link : links)
        {
            int source = link["source"];
            int target = link["target"];
            connectedNodeIds.insert(source);
            connectedNodeIds.insert(target);
        }

        // Count isolated players (nodes with no connections)
        int isolatedPlayers = 0;
        unordered_map<int, bool> visited;
        for (auto& node : nodes)
        {
            int nodeId = node["id"];
            if (connectedNodeIds.find(nodeId) == connectedNodeIds.end())
            {
                isolatedPlayers++;
            }
            visited[nodeId] = false;
        }

        // Build adjacency list for cluster detection
        unordered_map<int, vector<int>> adjacencyList;
        for (auto& node : nodes)
        {
            int nodeId = node["id"];
            adjacencyList[nodeId] = vector<int>();
        }
        for (auto& link : links)
        {
            int source = link["source"];
            int target = link["target"];
            adjacencyList[source].push_back(target);
            adjacencyList[target].push_back(source);
        }

        // DFS to find connected components (clusters)
        function<void(int, unordered_map<int, bool>&)> dfs = [&](int nodeId, unordered_map<int, bool>& vis)
        {
            vis[nodeId] = true;
            for (int neighbor : adjacencyList[nodeId])
            {
                if (!vis[neighbor])
                {
                    dfs(neighbor, vis);
                }
            }
        };

        int clusters = 0;
        for (auto& node : nodes)
        {
            int nodeId = node["id"];
            if (!visited[nodeId])
            {
                if (adjacencyList[nodeId].size() > 0)
                {
                    dfs(nodeId, visited);
                    clusters++;
                }
                else
                {
                    visited[nodeId] = true;
                    // isolated node counts as single cluster
                }
            }
        }

        json stats = {
            {"totalPlayers", (int)nodes.size()},
            {"totalConnections", totalConnections},
            {"isolatedPlayers", isolatedPlayers},
            {"clusters", clusters}
        };

        res.set_content(stats.dump(), "application/json");
    });

    server.Post("/addPlayer", [](const httplib::Request &req, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        auto body = json::parse(req.body);

        lock_guard<mutex> lock(dataMutex);
        leaderboard.addPlayer(body["username"], body["score"]);

        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    server.Post("/simulate", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);
        int cycles = 3 + rand() % 3;
        for (int i = 0; i < cycles; ++i) {
            leaderboard.simulateRandomUpdate();
        }
        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    server.Post("/api/update", [](const httplib::Request &req, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        auto body = json::parse(req.body);
        int id = body.value("player_id", 0);
        int change = body.value("score_change", 0);

        lock_guard<mutex> lock(dataMutex);
        leaderboard.updateScore(id, change);

        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    server.set_mount_point("/", "../frontend");

    cout << "Server running at http://localhost:8080\n";

    // Start background simulation thread
    // thread simulationThread([]() {
    //     while (true) {
    //         this_thread::sleep_for(chrono::seconds(10 + rand() % 6));  // 10-15 seconds
    //         lock_guard<mutex> lock(dataMutex);
    //         int cycles = 1 + rand() % 3;  // 1-3 updates
    //         for (int i = 0; i < cycles; ++i) {
    //             leaderboard.simulateRandomUpdate();
    //         }
    //     }
    // });
    // simulationThread.detach();

    server.listen("0.0.0.0", 8080);
}

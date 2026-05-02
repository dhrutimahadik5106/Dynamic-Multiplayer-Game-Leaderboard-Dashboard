#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>

#include "httplib.h"
#include "json.hpp"
#include "leaderboard.h"

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

json activityToJson(vector<string> activities)
{
    json arr = json::array();

    for(auto &a : activities)
    {
        arr.push_back(a);
    }

    return arr;
}


// ==========================
// Simulation Thread
// ==========================
void simulationEngine()
{
    while(true)
    {
        this_thread::sleep_for(chrono::seconds(2));

        lock_guard<mutex> lock(dataMutex);
        leaderboard.simulateRandomUpdate();
    }
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

    // LIVE SIMULATION THREAD
    thread simulationThread([](){
        while(true)
        {
            this_thread::sleep_for(chrono::seconds(3));

            dataMutex.lock();
            leaderboard.simulateRandomUpdate();
            dataMutex.unlock();
        }
    });
    simulationThread.detach();

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

    server.Get("/api/network", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        lock_guard<mutex> lock(dataMutex);
        res.set_content(leaderboard.getNetworkData().dump(), "application/json");
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

    server.listen("0.0.0.0", 8080);
}

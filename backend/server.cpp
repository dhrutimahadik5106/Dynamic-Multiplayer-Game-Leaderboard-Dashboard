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
        lock_guard<mutex> lock(dataMutex);
        res.set_content(playersToJson(leaderboard.getLeaderboard()).dump(), "application/json");
    });

    server.Get("/topPlayers", [](const httplib::Request&, httplib::Response &res)
    {
        lock_guard<mutex> lock(dataMutex);
        res.set_content(playersToJson(leaderboard.getTopPlayers(5)).dump(), "application/json");
    });

    server.Get("/activity", [](const httplib::Request&, httplib::Response &res)
    {
        lock_guard<mutex> lock(dataMutex);
        res.set_content(activityToJson(leaderboard.getActivity()).dump(), "application/json");
    });

    server.Get("/stats", [](const httplib::Request&, httplib::Response &res)
    {
        lock_guard<mutex> lock(dataMutex);

        auto players = leaderboard.getLeaderboard();

        json stats = {
            {"totalPlayers", (int)players.size()},
            {"topScore", players.empty()?0:players[0].score}
        };

        res.set_content(stats.dump(), "application/json");
    });

    server.Get("/api/network", [](const httplib::Request&, httplib::Response &res)
    {
        lock_guard<mutex> lock(dataMutex);
        res.set_content(leaderboard.getNetworkData().dump(), "application/json");
    });

    server.Post("/addPlayer", [](const httplib::Request &req, httplib::Response &res)
    {
        auto body = json::parse(req.body);

        lock_guard<mutex> lock(dataMutex);
        leaderboard.addPlayer(body["username"], body["score"]);

        res.set_content("{\"status\":\"ok\"}", "application/json");
    });

    server.set_mount_point("/", "../frontend");

    cout << "Server running at http://localhost:8080\n";

    server.listen("0.0.0.0", 8080);
}

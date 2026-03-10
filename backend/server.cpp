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
// Convert players to JSON
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
            {"score", p.score},
            {"status", "online"}
        });
    }

    return arr;
}


// ==========================
// Convert activity list
// ==========================
json activityToJson(vector<string> activity)
{
    json arr = json::array();

    for(auto &a : activity)
        arr.push_back(a);

    return arr;
}


// ==========================
// Simulation Engine
// ==========================
void simulationEngine()
{
    while(true)
    {
        this_thread::sleep_for(chrono::seconds(3));

        dataMutex.lock();
        leaderboard.simulateRandomUpdate();
        dataMutex.unlock();
    }
}


int main()
{
    srand(time(0));

    cout << "Dynamic Multiplayer Leaderboard Server\n";

    // Initial players
    leaderboard.addPlayer("Alice",1200);
    leaderboard.addPlayer("Bob",1100);
    leaderboard.addPlayer("Mike",980);
    leaderboard.addPlayer("Emma",920);

    // Start simulation thread
    thread sim(simulationEngine);
    sim.detach();

    httplib::Server server;

    // ==========================
    // GLOBAL CORS HEADERS (FIX)
    // ==========================
    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });


    // ==========================
    // LEADERBOARD API
    // ==========================
    server.Get("/api/leaderboard",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        auto players = leaderboard.getLeaderboard();
        dataMutex.unlock();

        res.set_content(
            playersToJson(players).dump(4),
            "application/json");
    });


    // ==========================
    // TOP PLAYERS
    // ==========================
    server.Get("/api/top",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        auto players = leaderboard.getTopPlayers(5);
        dataMutex.unlock();

        res.set_content(
            playersToJson(players).dump(4),
            "application/json");
    });


    // ==========================
    // ACTIVITY FEED
    // ==========================
    server.Get("/api/activity",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        auto activity = leaderboard.getActivity();
        dataMutex.unlock();

        res.set_content(
            activityToJson(activity).dump(4),
            "application/json");
    });


    // ==========================
    // ADD PLAYER
    // ==========================
    server.Post("/api/player",
    [](const httplib::Request &req, httplib::Response &res)
    {
        json body = json::parse(req.body);

        string username = body["username"];
        int score = body["score"];

        dataMutex.lock();
        Player p = leaderboard.addPlayer(username,score);
        dataMutex.unlock();

        json response = {
            {"status","success"},
            {"player_id",p.id}
        };

        res.set_content(response.dump(4),"application/json");
    });

    // ==========================
// GET PLAYER BY ID
// ==========================

server.Get(R"(/api/player/(\d+))",
[](const httplib::Request& req, httplib::Response& res)
{

    int id = stoi(req.matches[1]);

    Player p = leaderboard.getPlayerById(id);

    json response = {
        {"id",p.id},
        {"username",p.username},
        {"score",p.score}
    };

    res.set_header("Access-Control-Allow-Origin","*");
    res.set_content(response.dump(4),"application/json");

});


    // ==========================
    // UPDATE SCORE
    // ==========================
    server.Post("/api/update",
    [](const httplib::Request &req, httplib::Response &res)
    {
        json body = json::parse(req.body);

        int id = body["player_id"];
        int change = body["score_change"];

        dataMutex.lock();
        leaderboard.updateScore(id,change);
        dataMutex.unlock();

        json response = {
            {"status","updated"}
        };

        res.set_content(response.dump(4),"application/json");
    });




    // ==========================
    // STATS
    // ==========================
    server.Get("/api/stats",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();

        int total = leaderboard.totalPlayers();
        int top = leaderboard.topScore();
        double avg = leaderboard.avgScore();

        dataMutex.unlock();

        json response = {
            {"total_players", total},
            {"top_score", top},
            {"avg_score", avg}
        };

        res.set_content(response.dump(4),"application/json");
    });


    cout << "Server running on http://localhost:8080\n";

    server.listen("0.0.0.0",8080);

    return 0;
}


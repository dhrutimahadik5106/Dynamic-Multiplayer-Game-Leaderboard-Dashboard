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

    // Players are now generated in Leaderboard constructor (1000 players)

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
    server.Get("/leaderboard",
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
    server.Get("/topPlayers",
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
    server.Get("/activity",
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
    // STATS
    // ==========================
    server.Get("/stats",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        auto players = leaderboard.getLeaderboard();
        auto activity = leaderboard.getActivity();
        dataMutex.unlock();

        // Calculate statistics
        json stats = {
            {"totalPlayers", (int)players.size()},
            {"totalGames", (int)activity.size()},
            {"activeSessions", (int)players.size()},
            {"peakConcurrent", (int)players.size()},
            {"uptime", 99.9}
        };

        // Calculate average score
        double avgScore = 0;
        if(!players.empty()) {
            for(auto &p : players) avgScore += p.score;
            avgScore /= players.size();
        }
        stats["averageScore"] = avgScore;

        // Top score
        int topScore = players.empty() ? 0 : players[0].score;
        stats["topScore"] = topScore;

        json response = {{"stats", stats}};
        res.set_content(response.dump(4),"application/json");
    });


    // ==========================
    // DATA STRUCTURES
    // ==========================
    server.Get("/datastructures",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        json dsStats = leaderboard.getDataStructureStats();
        dataMutex.unlock();

        json response = {{"datastructures", dsStats}};
        res.set_content(response.dump(4),"application/json");
    });


    // ==========================
    // MATCHES (Player Network)
    // ==========================
    server.Get("/matches",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        auto players = leaderboard.getLeaderboard();
        dataMutex.unlock();

        // Create matches based on score proximity
        json matches = json::array();
        for(size_t i = 0; i < players.size(); ++i) {
            for(size_t j = i + 1; j < players.size() && j < i + 3; ++j) { // Connect to next 2 players
                matches.push_back({
                    {"source", players[i].username},
                    {"target", players[j].username}
                });
            }
        }

        res.set_content(matches.dump(4),"application/json");
    });


    // ==========================
    // ADD PLAYER
    // ==========================
    server.Post("/addPlayer",
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

        int player_id = body["player_id"];
        int score_change = body["score_change"];

        dataMutex.lock();
        leaderboard.updateScore(player_id,score_change);
        dataMutex.unlock();

        json response = {{"status","success"}};

        res.set_content(response.dump(4),"application/json");
    });

    // ==========================
    // SIMULATE UPDATE
    // ==========================
    server.Post("/api/simulate",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        leaderboard.simulateRandomUpdate();
        dataMutex.unlock();

        json response = {{"status","success"}};

        res.set_content(response.dump(4),"application/json");
    });

    // ==========================
    // NETWORK DATA
    // ==========================
    server.Get("/api/network",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        auto players = leaderboard.getLeaderboard();
        dataMutex.unlock();

        // Create nodes from players
        json nodes = json::array();
        for(size_t i = 0; i < players.size(); ++i) {
            nodes.push_back({
                {"id", players[i].id},
                {"name", players[i].username},
                {"score", players[i].score},
                {"rank", (int)i + 1}
            });
        }

        // Create links based on score proximity (simplified network)
        json links = json::array();
        for(size_t i = 0; i < players.size(); ++i) {
            for(size_t j = i + 1; j < players.size(); ++j) {
                int scoreDiff = abs(players[i].score - players[j].score);
                if(scoreDiff < 200) { // Connect players with similar scores
                    links.push_back({
                        {"source", players[i].id},
                        {"target", players[j].id},
                        {"value", max(1, 10 - scoreDiff / 20)}
                    });
                }
            }
        }

        json response = {
            {"nodes", nodes},
            {"links", links}
        };

        res.set_content(response.dump(4),"application/json");
    });

    // ==========================
    // STATISTICS DATA
    // ==========================
    server.Get("/api/stats",
    [](const httplib::Request&, httplib::Response &res)
    {
        dataMutex.lock();
        auto players = leaderboard.getLeaderboard();
        auto activity = leaderboard.getActivity();
        dataMutex.unlock();

        // Calculate statistics
        json stats = {
            {"totalPlayers", (int)players.size()},
            {"totalGames", (int)activity.size()},
            {"activeSessions", (int)players.size()}, // Simplified
            {"peakConcurrent", (int)players.size()}, // Simplified
            {"uptime", 99.9}
        };

        // Calculate average score
        double avgScore = 0;
        if(!players.empty()) {
            for(auto &p : players) avgScore += p.score;
            avgScore /= players.size();
        }
        stats["averageScore"] = avgScore;

        // Score distribution (simplified histogram)
        json scoreDist = {
            {"labels", {"0-200", "201-400", "401-600", "601-800", "801-1000", "1000+"}},
            {"data", {0, 0, 0, 0, 0, 0}}
        };
        for(auto &p : players) {
            int bucket = min(5, p.score / 200);
            scoreDist["data"][bucket] = (int)scoreDist["data"][bucket] + 1;
        }

        // Performance trends (simplified - last 10 activities)
        json trends = {
            {"labels", {}},
            {"data", {}}
        };
        vector<double> trendData;
        for(size_t i = 0; i < min((size_t)10, activity.size()); ++i) {
            trends["labels"].push_back("T-" + to_string(10-i));
            trendData.push_back(avgScore + (rand() % 100 - 50)); // Simulated trend
        }
        trends["data"] = trendData;

        // Rank distribution
        json rankDist = {0, 0, 0, 0}; // Top 10%, 25%, 50%, bottom 50%
        if(!players.empty()) {
            int top10 = players.size() / 10;
            int top25 = players.size() / 4;
            int top50 = players.size() / 2;
            rankDist[0] = top10;
            rankDist[1] = top25 - top10;
            rankDist[2] = top50 - top25;
            rankDist[3] = players.size() - top50;
        }

        // Activity timeline
        json timeline = {
            {"labels", {}},
            {"data", {}}
        };
        for(size_t i = 0; i < min((size_t)20, activity.size()); ++i) {
            timeline["labels"].push_back("Event " + to_string(i+1));
            timeline["data"].push_back(rand() % 10 + 1); // Simulated activity count
        }

        json response = {
            {"stats", stats},
            {"scoreDistribution", scoreDist},
            {"performanceTrends", trends},
            {"rankDistribution", rankDist},
            {"activityTimeline", timeline}
        };

        res.set_content(response.dump(4),"application/json");
    });

    // ==========================
    // STATIC FILE SERVING
    // ==========================
    server.set_mount_point("/", "../frontend");

    cout << "Server running on http://localhost:8080\n";
    cout << "Frontend available at: http://localhost:8080/index.html\n";
    server.listen("0.0.0.0", 8080);

    return 0;
}
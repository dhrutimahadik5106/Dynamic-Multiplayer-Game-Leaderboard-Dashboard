#include <iostream>
<<<<<<< HEAD
#include <thread>
#include <mutex>
#include <chrono>
#include <ctime>
#include <random>

#include "httplib.h"
#include "json.hpp"

#include <vector>
=======
#include <cstring>
#include <cstdlib>
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7
#include <string>
#include <fstream>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>

<<<<<<< HEAD
// Thread-safe random number generation
std::random_device rd;
std::mt19937 gen(rd());
std::mutex randMutex;

int getRandomInt(int min, int max) {
    std::lock_guard<std::mutex> lock(randMutex);
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

// Declarations
=======
#pragma comment(lib, "ws2_32.lib")
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7

#include "leaderboard.h"

<<<<<<< HEAD
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
    nlohmann::json getNetworkData();
    nlohmann::json getStatsData();
};
=======
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7

using namespace std;

Leaderboard leaderboard;

void intToString(int num, char* buffer)
{
    if(num == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    int isNegative = 0;
    if(num < 0)
    {
        isNegative = 1;
        num = -num;
    }

    char temp[50];
    int pos = 0;

    while(num > 0)
    {
        temp[pos] = (num % 10) + '0';
        num /= 10;
        pos++;
    }

    int bufPos = 0;
    if(isNegative)
    {
        buffer[bufPos] = '-';
        bufPos++;
    }

    for(int i = pos - 1; i >= 0; i--)
    {
        buffer[bufPos] = temp[i];
        bufPos++;
    }
    buffer[bufPos] = '\0';
}

void doubleToString(double num, char* buffer)
{
    int intPart = (int)num;
    int decPart = (int)((num - intPart) * 100);

    if(decPart < 0)
        decPart = -decPart;

<<<<<<< HEAD
    string name = baseNames[getRandomInt(0, baseNames.size() - 1)];
    string suffix = suffixes[getRandomInt(0, suffixes.size() - 1)];
    int number = getRandomInt(10, 99);
=======
    char intStr[50];
    intToString(intPart, intStr);
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7

    int i = 0;
    while(intStr[i] != '\0')
    {
        buffer[i] = intStr[i];
        i++;
    }

    buffer[i] = '.';
    i++;

    if(decPart < 10)
    {
        buffer[i] = '0';
        i++;
    }

    char decStr[50];
    intToString(decPart, decStr);

    int j = 0;
    while(decStr[j] != '\0')
    {
        buffer[i] = decStr[j];
        i++;
        j++;
    }
    buffer[i] = '\0';
}

string escapeJson(const char* text)
{
    string out = "";
    int i = 0;

    while(text[i] != '\0')
    {
        char c = text[i];
        if(c == '"' || c == '\\')
        {
            out += '\\';
            out += c;
        }
        else if(c == '\n')
        {
            out += "\\n";
        }
        else if(c == '\r')
        {
            out += "\\r";
        }
        else
        {
            out += c;
        }
        i++;
    }

    return out;
}

void getJsonLeaderboard(string& output)
{
    Player result[500];
    int count = 0;
    leaderboard.getLeaderboard(result, &count);

    output = "[";

    for(int i = 0; i < count; i++)
    {
        char rank[10];
        intToString(i + 1, rank);

        output += "{\"rank\":";
        output += rank;
        output += ",\"id\":";

        char idStr[20];
        intToString(result[i].id, idStr);
        output += idStr;

        output += ",\"username\":\"";
        output += escapeJson(result[i].username);
        output += "\",\"score\":";

        char scoreStr[20];
        intToString(result[i].score, scoreStr);
        output += scoreStr;
        output += "}";

        if(i < count - 1)
            output += ",";
    }

    output += "]";
}

void getJsonTopPlayers(string& output)
{
    Player result[10];
    int count = 0;
    leaderboard.getTopPlayers(5, result, &count);

    output = "[";

    for(int i = 0; i < count; i++)
    {
        char rank[10];
        intToString(i + 1, rank);

        output += "{\"rank\":";
        output += rank;
        output += ",\"id\":";

        char idStr[20];
        intToString(result[i].id, idStr);
        output += idStr;

        output += ",\"username\":\"";
        output += escapeJson(result[i].username);
        output += "\",\"score\":";

        char scoreStr[20];
        intToString(result[i].score, scoreStr);
        output += scoreStr;
        output += "}";

        if(i < count - 1)
            output += ",";
    }

    output += "]";
}

void getJsonActivity(string& output)
{
    char activities[100][200];
    int count = 0;
    leaderboard.getActivity(activities, &count);

    output = "{\"events\":[";

    for(int i = 0; i < count; i++)
    {
        output += "{\"type\":\"activity\",\"text\":\"";
        output += escapeJson(activities[i]);
        output += "\"}";

        if(i < count - 1)
            output += ",";
    }

    output += "],\"totalEvents\":";
    char totalStr[10];
    intToString(count, totalStr);
    output += totalStr;
    output += "}";
}

void getJsonStats(string& output)
{
    int totalPlayers = leaderboard.getTotalPlayers();
    int topScore = leaderboard.getTopScore();
    double avgScore = leaderboard.getAvgScore();

    char avgStr[50];
    doubleToString(avgScore, avgStr);

    char topScoreStr[20];
    intToString(topScore, topScoreStr);

    char totalStr[20];
    intToString(totalPlayers, totalStr);

    output = "{\"totalPlayers\":";
    output += totalStr;
    output += ",\"topScore\":";
    output += topScoreStr;
    output += ",\"avgScore\":";
    output += avgStr;
    output += ",\"activeMatches\":25}";
}

void getJsonNetwork(string& output)
{
    Player players[500];
    int count = 0;
    leaderboard.getLeaderboard(players, &count);

    int n = count;
    if(n > 50)
        n = 50;

    output = "{\"nodes\":[";

    for(int i = 0; i < n; i++)
    {
        output += "{\"id\":";
        char idStr[20];
        intToString(players[i].id, idStr);
        output += idStr;

        output += ",\"name\":\"";
        output += escapeJson(players[i].username);
        output += "\",\"score\":";
        char scoreStr[20];
        intToString(players[i].score, scoreStr);
        output += scoreStr;

        output += ",\"rank\":";
        char rankStr[10];
        intToString(i + 1, rankStr);
        output += rankStr;
        output += "}";

        if(i < n - 1)
            output += ",";
    }

    output += "],\"links\":[";

    int firstLink = 1;
    for(int i = 0; i < n; i++)
    {
        if(i + 1 < n)
        {
            if(!firstLink) output += ",";
            firstLink = 0;
            output += "{\"source\":";
            char s1[20];
            intToString(players[i].id, s1);
            output += s1;
            output += ",\"target\":";
            char t1[20];
            intToString(players[i + 1].id, t1);
            output += t1;
            output += ",\"value\":1}";
        }

        if(i + 2 < n)
        {
            output += ",";
            output += "{\"source\":";
            char s2[20];
            intToString(players[i].id, s2);
            output += s2;
            output += ",\"target\":";
            char t2[20];
            intToString(players[i + 2].id, t2);
            output += t2;
            output += ",\"value\":1}";
        }
    }

    output += "]}";
}

void getJsonApiStats(string& output)
{
    Player players[500];
    int count = 0;
    leaderboard.getLeaderboard(players, &count);

    Player topPlayers[10];
    int topCount = 0;
    leaderboard.getTopPlayers(10, topPlayers, &topCount);

    char activityLog[100][200];
    int activityCount = 0;
    leaderboard.getActivity(activityLog, &activityCount);

    int totalPlayers = count;
    int topScore = leaderboard.getTopScore();
    double avgScore = leaderboard.getAvgScore();

    int recordsBroken = 0;
    for(int i = 0; i < count; i++)
    {
        if(players[i].score == topScore)
            recordsBroken++;
    }

    int b1 = 0;
    int b2 = 0;
    int b3 = 0;
    int b4 = 0;
    int b5 = 0;

    for(int i = 0; i < count; i++)
    {
        int s = players[i].score;
        if(s < 500) b1++;
        else if(s < 1000) b2++;
        else if(s < 1500) b3++;
        else if(s < 2000) b4++;
        else b5++;
    }

    int top10 = (int)(totalPlayers * 0.10);
    if(top10 < 1 && totalPlayers > 0) top10 = 1;
    int top25 = (int)(totalPlayers * 0.25);
    if(top25 < top10) top25 = top10;
    int top50 = (int)(totalPlayers * 0.50);
    if(top50 < top25) top50 = top25;

    int d1 = top10;
    int d2 = top25 - top10;
    int d3 = top50 - top25;
    int d4 = totalPlayers - top50;
    if(d4 < 0) d4 = 0;

    char totalStr[20];
    intToString(totalPlayers, totalStr);
    char gamesStr[20];
    intToString(totalPlayers * 3, gamesStr);
    char topStr[20];
    intToString(topScore, topStr);
    char avgStr[40];
    doubleToString(avgScore, avgStr);
    char dbSizeStr[20];
    doubleToString(5.0 + (double)totalPlayers * 0.01, dbSizeStr);

    char b1s[10], b2s[10], b3s[10], b4s[10], b5s[10];
    intToString(b1, b1s); intToString(b2, b2s); intToString(b3, b3s); intToString(b4, b4s); intToString(b5, b5s);

    char d1s[10], d2s[10], d3s[10], d4s[10];
    intToString(d1, d1s); intToString(d2, d2s); intToString(d3, d3s); intToString(d4, d4s);

    output = "{";
    output += "\"scoreDistribution\":{\"labels\":[\"0-499\",\"500-999\",\"1000-1499\",\"1500-1999\",\"2000+\"],\"data\":[";
    output += b1s; output += ","; output += b2s; output += ","; output += b3s; output += ","; output += b4s; output += ","; output += b5s; output += "]},";

    output += "\"performanceTrends\":{\"labels\":[\"T-5\",\"T-4\",\"T-3\",\"T-2\",\"T-1\",\"Now\"],\"data\":[";
    output += avgStr; output += ","; output += avgStr; output += ","; output += avgStr; output += ","; output += avgStr; output += ","; output += avgStr; output += ","; output += avgStr; output += "]},";

    output += "\"rankDistribution\":[";
    output += d1s; output += ","; output += d2s; output += ","; output += d3s; output += ","; output += d4s; output += "],";

    output += "\"activityTimeline\":{\"labels\":[\"1\",\"2\",\"3\",\"4\",\"5\",\"6\"],\"data\":[2,4,3,5,4,6]},";

    output += "\"topPlayers\":[";
    for(int i = 0; i < topCount; i++)
    {
        output += "{\"id\":";
        char idStr[20];
        intToString(topPlayers[i].id, idStr);
        output += idStr;
        output += ",\"username\":\"";
        output += escapeJson(topPlayers[i].username);
        output += "\",\"score\":";
        char scoreStr[20];
        intToString(topPlayers[i].score, scoreStr);
        output += scoreStr;
        output += "}";
        if(i < topCount - 1)
            output += ",";
    }
    output += "],";

    output += "\"stats\":{\"totalPlayers\":";
    output += totalStr;
    output += ",\"totalGames\":";
    output += gamesStr;
    output += ",\"averageScore\":";
    output += avgStr;
    output += ",\"activeSessions\":";
    char activeStr[20];
    intToString((totalPlayers / 2) + 1, activeStr);
    output += activeStr;
    output += ",\"peakConcurrent\":";
    output += totalStr;
    output += ",\"uptime\":99,\"topScore\":";
    output += topStr;
    output += "}";

    output += ",\"recordsBroken\":";
    char brokenStr[20];
    intToString(recordsBroken, brokenStr);
    output += brokenStr;

    output += ",\"dbPlayers\":";
    output += totalStr;
    output += ",\"dbMatches\":";
    output += gamesStr;
    output += ",\"dbActivity\":";
    char activityStr[20];
    intToString(activityCount, activityStr);
    output += activityStr;
    output += ",\"dbSize\":";
    output += dbSizeStr;

    output += "}";
}

int parseIntField(const string& body, const string& key, int defaultValue)
{
    size_t keyPos = body.find("\"" + key + "\"");
    if(keyPos == string::npos)
        return defaultValue;

    size_t colonPos = body.find(':', keyPos);
    if(colonPos == string::npos)
        return defaultValue;

    size_t valueStart = colonPos + 1;
    while(valueStart < body.size() && (body[valueStart] == ' ' || body[valueStart] == '\t'))
        valueStart++;

    int sign = 1;
    if(valueStart < body.size() && body[valueStart] == '-')
    {
        sign = -1;
        valueStart++;
    }

    int value = 0;
    int foundDigit = 0;
    while(valueStart < body.size() && body[valueStart] >= '0' && body[valueStart] <= '9')
    {
        value = value * 10 + (body[valueStart] - '0');
        valueStart++;
        foundDigit = 1;
    }

    if(!foundDigit)
        return defaultValue;

    return value * sign;
}

void parseStringField(const string& body, const string& key, char* output, int maxLen)
{
    output[0] = '\0';

    size_t keyPos = body.find("\"" + key + "\"");
    if(keyPos == string::npos)
        return;

    size_t colonPos = body.find(':', keyPos);
    if(colonPos == string::npos)
        return;

    size_t quoteStart = body.find('"', colonPos + 1);
    if(quoteStart == string::npos)
        return;

    size_t quoteEnd = body.find('"', quoteStart + 1);
    if(quoteEnd == string::npos)
        return;

    int j = 0;
    for(size_t i = quoteStart + 1; i < quoteEnd && j < maxLen - 1; i++)
    {
        output[j] = body[i];
        j++;
    }
    output[j] = '\0';
}

void seedPlayers()
{
    char name1[] = "ShadowX";
    char name2[] = "NovaHunter";
    char name3[] = "DragonKing";
    char name4[] = "PhantomStrike";
    char name5[] = "CyberNinja";
    char name6[] = "DarkBlade";
    char name7[] = "IceStorm";
    char name8[] = "StormRider";
    char name9[] = "GhostThunder";
    char name10[] = "SilverClaw";

    leaderboard.addPlayer(name1, 567);
    leaderboard.addPlayer(name2, 600);
    leaderboard.addPlayer(name3, 1824);
    leaderboard.addPlayer(name4, 1458);
    leaderboard.addPlayer(name5, 927);
    leaderboard.addPlayer(name6, 245);
    leaderboard.addPlayer(name7, 591);
    leaderboard.addPlayer(name8, 564);
    leaderboard.addPlayer(name9, 2042);
    leaderboard.addPlayer(name10, 1536);
}

string getContentType(const string& path)
{
    if(path.find(".html") != string::npos) return "text/html";
    if(path.find(".css") != string::npos) return "text/css";
    if(path.find(".js") != string::npos) return "application/javascript";
    if(path.find(".json") != string::npos) return "application/json";
    return "text/plain";
}

bool readFileText(const string& path, string& out)
{
    ifstream file(path.c_str(), ios::binary);
    if(!file.is_open()) return false;

    stringstream ss;
    ss << file.rdbuf();
    out = ss.str();
    return true;
}

bool readFrontendFile(const string& routePath, string& out, string& resolvedPath)
{
    string rel = routePath;
    if(rel == "/" || rel == "")
        rel = "/index.html";

    string p1 = string("frontend") + rel;
    if(readFileText(p1, out))
    {
        resolvedPath = p1;
        return true;
    }

    string p2 = string("../frontend") + rel;
    if(readFileText(p2, out))
    {
        resolvedPath = p2;
        return true;
    }

    return false;
}

void sendHttp(SOCKET client, const string& status, const string& contentType, const string& body)
{
    string response = "HTTP/1.1 " + status + "\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    response += "Access-Control-Allow-Headers: Content-Type\r\n";
    response += "Content-Length: ";

    char lenStr[20];
    intToString((int)body.size(), lenStr);
    response += lenStr;
    response += "\r\nConnection: close\r\n\r\n";
    response += body;

    send(client, response.c_str(), (int)response.size(), 0);
}

int main()
{
<<<<<<< HEAD
    cout << "Server starting...\n";
=======
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        cout << "WSAStartup failed" << endl;
        return 1;
    }

    seedPlayers();
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7

    SOCKET serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serverSock == INVALID_SOCKET)
    {
        cout << "Socket create failed" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if(bind(serverSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cout << "Bind failed" << endl;
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    if(listen(serverSock, 10) == SOCKET_ERROR)
    {
        cout << "Listen failed" << endl;
        closesocket(serverSock);
        WSACleanup();
        return 1;
    }

    cout << "Server running at http://localhost:8080" << endl;

    while(true)
    {
        SOCKET client = accept(serverSock, NULL, NULL);
        if(client == INVALID_SOCKET)
            continue;

        char buffer[8192];
        int received = recv(client, buffer, sizeof(buffer) - 1, 0);
        if(received <= 0)
        {
<<<<<<< HEAD
            try {
                this_thread::sleep_for(chrono::seconds(10));

                lock_guard<mutex> lock(dataMutex);

                int joins = 3 + getRandomInt(0, 1); // 3-4 new players
                for (int i = 0; i < joins; ++i) {
                    string username = generateRandomUsername();
                    int score = 100 + getRandomInt(0, 2100); // 100-2200
                    leaderboard.addPlayer(username, score);
                }

                leaderboard.simulateRandomUpdate();
            } catch (const exception& e) {
                cout << "Error in live update thread: " << e.what() << endl;
            }
        }
    });
    liveUpdateThread.detach();

    // ================= API =================

    server.Get("/leaderboard", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            lock_guard<mutex> lock(dataMutex);
            res.set_content(playersToJson(leaderboard.getLeaderboard()).dump(), "application/json");
        } catch (const exception& e) {
            res.status = 500;
            res.set_content("Internal Server Error: " + string(e.what()), "text/plain");
        }
    });

    server.Get("/topPlayers", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            lock_guard<mutex> lock(dataMutex);
            res.set_content(playersToJson(leaderboard.getTopPlayers(5)).dump(), "application/json");
        } catch (const exception& e) {
            res.status = 500;
            res.set_content("Internal Server Error: " + string(e.what()), "text/plain");
        }
    });

    server.Get("/activity", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            lock_guard<mutex> lock(dataMutex);
            res.set_content(activityToJson(leaderboard.getActivity()).dump(), "application/json");
        } catch (const exception& e) {
            res.status = 500;
            res.set_content("Internal Server Error: " + string(e.what()), "text/plain");
        }
    });

    server.Get("/stats", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            lock_guard<mutex> lock(dataMutex);
            auto data = leaderboard.getStatsData();
            std::cout << data.dump(2) << std::endl;
            res.set_content(data.dump(), "application/json");
        } catch (const exception& e) {
            res.status = 500;
            res.set_content("Internal Server Error: " + string(e.what()), "text/plain");
        }
    });

    server.Get("/api/stats", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            lock_guard<mutex> lock(dataMutex);
            res.set_content(leaderboard.getStatsData().dump(), "application/json");
        } catch (const exception& e) {
            res.status = 500;
            res.set_content("Internal Server Error: " + string(e.what()), "text/plain");
        }
    });

    server.Get("/statsData", [](const httplib::Request&, httplib::Response &res)
    {
        res.set_header("Access-Control-Allow-Origin", "*");
        try {
            lock_guard<mutex> lock(dataMutex);
            res.set_content(leaderboard.getStatsData().dump(), "application/json");
        } catch (const exception& e) {
            res.status = 500;
            res.set_content("Internal Server Error: " + string(e.what()), "text/plain");
        }
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
=======
            closesocket(client);
            continue;
        }
        buffer[received] = '\0';

        string req(buffer);

        size_t lineEnd = req.find("\r\n");
        string requestLine = (lineEnd == string::npos) ? req : req.substr(0, lineEnd);

        string method = "";
        string path = "";

        size_t p1 = requestLine.find(' ');
        if(p1 != string::npos)
        {
            method = requestLine.substr(0, p1);
            size_t p2 = requestLine.find(' ', p1 + 1);
            if(p2 != string::npos)
                path = requestLine.substr(p1 + 1, p2 - p1 - 1);
        }

        string body = "";
        size_t bodyPos = req.find("\r\n\r\n");
        if(bodyPos != string::npos)
            body = req.substr(bodyPos + 4);

        if(method == "OPTIONS")
        {
            sendHttp(client, "200 OK", "application/json", "{}");
            closesocket(client);
            continue;
        }

        if(method == "GET" && path == "/leaderboard")
        {
            string out;
            getJsonLeaderboard(out);
            sendHttp(client, "200 OK", "application/json", out);
        }
        else if(method == "GET" && path == "/topPlayers")
        {
            string out;
            getJsonTopPlayers(out);
            sendHttp(client, "200 OK", "application/json", out);
        }
        else if(method == "GET" && path == "/activity")
        {
            string out;
            getJsonActivity(out);
            sendHttp(client, "200 OK", "application/json", out);
        }
        else if(method == "GET" && path == "/stats")
        {
            string out;
            getJsonStats(out);
            sendHttp(client, "200 OK", "application/json", out);
        }
        else if(method == "GET" && path == "/api/network")
        {
            string out;
            getJsonNetwork(out);
            sendHttp(client, "200 OK", "application/json", out);
        }
        else if(method == "GET" && (path == "/api/stats" || path == "/statsData"))
        {
            string out;
            getJsonApiStats(out);
            sendHttp(client, "200 OK", "application/json", out);
        }
        else if(method == "POST" && path == "/addPlayer")
        {
            char username[50];
            parseStringField(body, "username", username, 50);
            int score = parseIntField(body, "score", 0);

            if(username[0] == '\0')
            {
                sendHttp(client, "400 Bad Request", "application/json", "{\"status\":\"error\",\"message\":\"username required\"}");
            }
            else
            {
                leaderboard.addPlayer(username, score);
                sendHttp(client, "200 OK", "application/json", "{\"status\":\"ok\"}");
            }
        }
        else if(method == "POST" && path == "/simulate")
>>>>>>> 32006fd7b88c7d28eff3f2ec53b06997290b94c7
        {
            for(int i = 0; i < 3; i++)
                leaderboard.simulateRandomUpdate();
            sendHttp(client, "200 OK", "application/json", "{\"status\":\"ok\"}");
        }
        else if(method == "POST" && path == "/api/update")
        {
            int id = parseIntField(body, "player_id", 0);
            int change = parseIntField(body, "score_change", 0);

            if(id <= 0)
                sendHttp(client, "400 Bad Request", "application/json", "{\"status\":\"error\",\"message\":\"invalid player_id\"}");
            else
            {
                leaderboard.updateScore(id, change);
                sendHttp(client, "200 OK", "application/json", "{\"status\":\"ok\"}");
            }

            cout << "UPDATE API HIT" << endl;
            cout << "BODY: " << body << endl;
        }
        else
        {
            string filePath;
            string fileBody;
            if(readFrontendFile(path, fileBody, filePath))
                sendHttp(client, "200 OK", getContentType(filePath), fileBody);
            else
                sendHttp(client, "404 Not Found", "text/plain", "Not Found");
        }

        closesocket(client);
    }

    closesocket(serverSock);
    WSACleanup();
    return 0;
}

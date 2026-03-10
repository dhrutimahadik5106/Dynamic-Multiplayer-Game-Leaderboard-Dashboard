#include "database.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;


/*-------------------------------------------------------
    Constructor
-------------------------------------------------------*/

Database::Database(string filename)
{
    this->filename = filename;
}


/*-------------------------------------------------------
    Save Players to File
-------------------------------------------------------*/

void Database::save(vector<Player> players)
{
    ofstream file(filename);

    if (!file.is_open())
    {
        cout << "Error opening database file for writing\n";
        return;
    }

    for (auto &p : players)
    {
        file << p.name << "," << p.score << endl;
    }

    file.close();
}


/*-------------------------------------------------------
    Load Players from File
-------------------------------------------------------*/

vector<Player> Database::load()
{
    vector<Player> players;

    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Database file not found. Creating new leaderboard.\n";
        return players;
    }

    string line;

    while (getline(file, line))
    {
        stringstream ss(line);

        string name;
        string scoreStr;

        getline(ss, name, ',');
        getline(ss, scoreStr);

        Player p;
        p.name = name;
        p.score = stoi(scoreStr);

        players.push_back(p);
    }

    file.close();

    return players;
}


/*-------------------------------------------------------
    Clear Database
-------------------------------------------------------*/

void Database::clear()
{
    ofstream file(filename, ios::trunc);

    file.close();
}

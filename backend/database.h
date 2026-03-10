#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>

using namespace std;


/*-------------------------------------------------------
   Player Structure
-------------------------------------------------------*/

struct Player
{
    string name;
    int score;
};


/*-------------------------------------------------------
   Database Class
-------------------------------------------------------*/

class Database
{
private:

    string filename;


public:

    Database(string filename);

    void save(vector<Player> players);

    vector<Player> load();

    void clear();
};

#endif

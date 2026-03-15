#ifndef ACTIVITY_LIST_H
#define ACTIVITY_LIST_H

#include <string>
#include <vector>


using namespace std;


/*-------------------------------------------------------
   Activity Node (Doubly Linked List)
-------------------------------------------------------*/

struct ActivityNode
{
    string message;

    ActivityNode* prev;
    ActivityNode* next;
};


/*-------------------------------------------------------
   Activity List Class
-------------------------------------------------------*/

class ActivityList
{

private:

    ActivityNode* head;
    ActivityNode* tail;

    int maxSize;
    int currentSize;

    ActivityNode* createNode(string message);

    void removeLast();


public:

    ActivityList();

    void addActivity(string message);

    vector<string> getActivities();

    void clear();
};

#endif

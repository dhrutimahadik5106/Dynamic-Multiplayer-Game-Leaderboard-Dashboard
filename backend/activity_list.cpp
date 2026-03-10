#include "activity_list.h"

using namespace std;


/*-------------------------------------------------------
   Constructor
-------------------------------------------------------*/

ActivityList::ActivityList()
{
    head = nullptr;
    tail = nullptr;
    maxSize = 20;
    currentSize = 0;
}


/*-------------------------------------------------------
   Create New Node
-------------------------------------------------------*/

ActivityNode* ActivityList::createNode(string message)
{
    ActivityNode* node = new ActivityNode;

    node->message = message;
    node->prev = nullptr;
    node->next = nullptr;

    return node;
}


/*-------------------------------------------------------
   Add Activity (Insert at Front)
-------------------------------------------------------*/

void ActivityList::addActivity(string message)
{
    ActivityNode* node = createNode(message);

    if(head == nullptr)
    {
        head = node;
        tail = node;
    }
    else
    {
        node->next = head;
        head->prev = node;
        head = node;
    }

    currentSize++;

    /* Remove oldest activity if limit exceeded */
    if(currentSize > maxSize)
    {
        removeLast();
    }
}


/*-------------------------------------------------------
   Remove Last Activity
-------------------------------------------------------*/

void ActivityList::removeLast()
{
    if(tail == nullptr)
        return;

    ActivityNode* temp = tail;

    tail = tail->prev;

    if(tail != nullptr)
        tail->next = nullptr;
    else
        head = nullptr;

    delete temp;

    currentSize--;
}


/*-------------------------------------------------------
   Get Activity Feed
-------------------------------------------------------*/

vector<string> ActivityList::getActivities()
{
    vector<string> activities;

    ActivityNode* current = head;

    while(current != nullptr)
    {
        activities.push_back(current->message);
        current = current->next;
    }

    return activities;
}


/*-------------------------------------------------------
   Clear Activity List
-------------------------------------------------------*/

void ActivityList::clear()
{
    ActivityNode* current = head;

    while(current != nullptr)
    {
        ActivityNode* temp = current;
        current = current->next;
        delete temp;
    }

    head = nullptr;
    tail = nullptr;
    currentSize = 0;
}

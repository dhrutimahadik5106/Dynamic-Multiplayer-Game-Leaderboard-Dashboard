#ifndef ACTIVITY_LIST_H
#define ACTIVITY_LIST_H

struct ActivityNode
{
    char message[200];
    ActivityNode* prev;
    ActivityNode* next;
};

class ActivityList
{
private:
    ActivityNode* head;
    ActivityNode* tail;
    int currentSize;   
    int maxSize;       

public:
    ActivityList();   

    void addActivity(char* message);
    void getActivities(char activities[][200], int* count);
    void clear();
};

#endif
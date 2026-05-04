#include <iostream>
#include <cstring>
#include <cstdlib>
#include "activity_list.h"

ActivityList::ActivityList()
{
    head = 0;
    tail = 0;
    currentSize = 0;
    maxSize = 20;
}

using namespace std;


void ActivityList::addActivity(char* message)
{
    ActivityNode* node = new ActivityNode;
    
    int i = 0;
    while(message[i] != '\0' && i < 199)
    {
        node->message[i] = message[i];
        i++;
    }
    node->message[i] = '\0';
    
    node->prev = 0;
    node->next = 0;
    
    if(head == 0)
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
    
    if(currentSize > maxSize)
    {
        if(tail == 0)
            return;
        
        ActivityNode* temp = tail;
        tail = tail->prev;
        
        if(tail != 0)
            tail->next = 0;
        else
            head = 0;
        
        delete temp;
        currentSize--;
    }
}

void ActivityList::getActivities(char activities[][200], int* count)
{
    ActivityNode* current = head;
    int index = 0;
    
    while(current != 0)
    {
        int i = 0;
        while(current->message[i] != '\0' && i < 199)
        {
            activities[index][i] = current->message[i];
            i++;
        }
        activities[index][i] = '\0';
        
        current = current->next;
        index++;
    }
    
    *count = index;
}

void ActivityList::clear()
{
    ActivityNode* current = head;
    
    while(current != 0)
    {
        ActivityNode* temp = current;
        current = current->next;
        delete temp;
    }
    
    head = 0;
    tail = 0;
    currentSize = 0;
}

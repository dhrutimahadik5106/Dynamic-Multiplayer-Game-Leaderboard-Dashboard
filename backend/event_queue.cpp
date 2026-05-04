#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

struct Event
{
    char message[200];
};

struct QueueNode
{
    Event event;
    QueueNode* next;
};

class EventQueue
{
private:
    QueueNode* front;
    QueueNode* rear;
    
public:
    EventQueue();
    void push(Event e);
    Event pop();
    int isEmpty();
};

EventQueue::EventQueue()
{
    front = 0;
    rear = 0;
}

void EventQueue::push(Event e)
{
    QueueNode* node = new QueueNode;
    node->event = e;
    node->next = 0;
    
    if(rear == 0)
    {
        front = node;
        rear = node;
    }
    else
    {
        rear->next = node;
        rear = node;
    }
}

Event EventQueue::pop()
{
    QueueNode* temp = front;
    Event e = front->event;
    front = front->next;
    
    if(front == 0)
        rear = 0;
    
    delete temp;
    return e;
}

int EventQueue::isEmpty()
{
    return front == 0;
}

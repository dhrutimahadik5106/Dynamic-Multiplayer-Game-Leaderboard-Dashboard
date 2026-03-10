#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#include <queue>
#include <string>

struct Event {
    std::string message;
};

class EventQueue {
private:
    std::queue<Event> q;

public:
    void push(Event e) {
        q.push(e);
    }

    Event pop() {
        Event e = q.front();
        q.pop();
        return e;
    }

    bool empty() {
        return q.empty();
    }
};

#endif

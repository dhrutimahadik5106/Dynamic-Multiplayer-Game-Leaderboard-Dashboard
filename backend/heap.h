#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include "player.h"

using namespace std;


/*-------------------------------------------------------
   Max Heap Class
-------------------------------------------------------*/

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

#endif
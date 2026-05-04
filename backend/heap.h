#ifndef HEAP_H
#define HEAP_H

#include "player.h"

class MaxHeap
{
private:
    Player heap[1000];
    int heapSize;

    int getParent(int i);
    int getLeft(int i);
    int getRight(int i);
    void swapPlayers(int i, int j);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    MaxHeap();
    void insert(Player player);
    Player getMax();
    Player extractMax();
    void getTopN(Player* topPlayers, int n, int* count);
    int getSize();
};

#endif
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

class MaxHeap
{
private:
    Player heap[1000];
    int heapSize;
    
    int getParent(int i);
    int getLeft(int i);
    int getRight(int i);
    void heapifyUp(int index);
    void heapifyDown(int index);
    void swapPlayers(int i, int j);
    
public:
    MaxHeap();
    void insert(Player player);
    Player getMax();
    Player extractMax();
    void getTopN(Player* topPlayers, int n, int* count);
    int getSize();
};

MaxHeap::MaxHeap()
{
    heapSize = 0;
}

int MaxHeap::getParent(int i)
{
    return (i - 1) / 2;
}

int MaxHeap::getLeft(int i)
{
    return 2 * i + 1;
}

int MaxHeap::getRight(int i)
{
    return 2 * i + 2;
}

void MaxHeap::swapPlayers(int i, int j)
{
    Player temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

void MaxHeap::heapifyUp(int index)
{
    while(index > 0)
    {
        int parent = getParent(index);
        if(heap[index].score > heap[parent].score)
        {
            swapPlayers(index, parent);
            index = parent;
        }
        else
            break;
    }
}

void MaxHeap::heapifyDown(int index)
{
    while(1)
    {
        int largest = index;
        int left = getLeft(index);
        int right = getRight(index);
        
        if(left < heapSize && heap[left].score > heap[largest].score)
            largest = left;
        
        if(right < heapSize && heap[right].score > heap[largest].score)
            largest = right;
        
        if(largest != index)
        {
            swapPlayers(index, largest);
            index = largest;
        }
        else
            break;
    }
}

void MaxHeap::insert(Player player)
{
    if(heapSize >= 1000)
        return;
    
    heap[heapSize] = player;
    heapifyUp(heapSize);
    heapSize++;
}

Player MaxHeap::getMax()
{
    return heap[0];
}

Player MaxHeap::extractMax()
{
    Player max = heap[0];
    heapSize--;
    heap[0] = heap[heapSize];
    heapifyDown(0);
    return max;
}

void MaxHeap::getTopN(Player* topPlayers, int n, int* count)
{
    if(n > heapSize)
        n = heapSize;
    
    MaxHeap temp;
    temp.heapSize = heapSize;
    for(int i = 0; i < heapSize; i++)
        temp.heap[i] = heap[i];
    
    int index = 0;
    while(index < n && temp.heapSize > 0)
    {
        topPlayers[index] = temp.extractMax();
        index++;
    }
    
    *count = index;
}

int MaxHeap::getSize()
{
    return heapSize;
}

#include "heap.h"
#include <algorithm>

using namespace std;


/*-------------------------------------------------------
   Constructor
-------------------------------------------------------*/

MaxHeap::MaxHeap()
{
}


/*-------------------------------------------------------
   Get Parent Index
-------------------------------------------------------*/

int MaxHeap::parent(int i)
{
    return (i - 1) / 2;
}


/*-------------------------------------------------------
   Left Child Index
-------------------------------------------------------*/

int MaxHeap::left(int i)
{
    return (2 * i) + 1;
}


/*-------------------------------------------------------
   Right Child Index
-------------------------------------------------------*/

int MaxHeap::right(int i)
{
    return (2 * i) + 2;
}


/*-------------------------------------------------------
   Heapify Up
-------------------------------------------------------*/

void MaxHeap::heapifyUp(int index)
{
    while (index > 0 && heap[parent(index)].score < heap[index].score)
    {
        swap(heap[parent(index)], heap[index]);
        index = parent(index);
    }
}


/*-------------------------------------------------------
   Heapify Down
-------------------------------------------------------*/

void MaxHeap::heapifyDown(int index)
{
    int largest = index;
    int l = left(index);
    int r = right(index);

    if (l < heap.size() && heap[l].score > heap[largest].score)
        largest = l;

    if (r < heap.size() && heap[r].score > heap[largest].score)
        largest = r;

    if (largest != index)
    {
        swap(heap[index], heap[largest]);
        heapifyDown(largest);
    }
}


/*-------------------------------------------------------
   Insert Player into Heap
-------------------------------------------------------*/

void MaxHeap::insert(Player player)
{
    heap.push_back(player);

    int index = heap.size() - 1;

    heapifyUp(index);
}


/*-------------------------------------------------------
   Remove Top Player
-------------------------------------------------------*/

Player MaxHeap::extractMax()
{
    if (heap.empty())
        return Player();

    Player root = heap[0];

    heap[0] = heap.back();
    heap.pop_back();

    heapifyDown(0);

    return root;
}


/*-------------------------------------------------------
   Get Top Player
-------------------------------------------------------*/

Player MaxHeap::getMax()
{
    if (heap.empty())
        return Player();

    return heap[0];
}


/*-------------------------------------------------------
   Get Top N Players
-------------------------------------------------------*/

vector<Player> MaxHeap::getTopN(int n)
{
    vector<Player> result;

    vector<Player> temp = heap;

    sort(temp.begin(), temp.end(), [](Player a, Player b)
         {
             return a.score > b.score;
         });

    for (int i = 0; i < n && i < temp.size(); i++)
    {
        result.push_back(temp[i]);
    }

    return result;
}


/*-------------------------------------------------------
   Heap Size
-------------------------------------------------------*/

int MaxHeap::size()
{
    return heap.size();
}

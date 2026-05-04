#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "player.h"

struct Node
{
    Player data;
    Node* left;
    Node* right;
    int height;
};

class AVLTree
{
private:
    Node* root;

    int getHeight(Node* node);
    int getBalance(Node* node);
    Node* rotateRight(Node* y);
    Node* rotateLeft(Node* x);
    Node* insertNode(Node* node, Player player);
    Node* minValueNode(Node* node);
    void inorderWalk(Node* node, Player* players, int* index);

public:
    AVLTree();
    void insert(Player player);
    void getSortedPlayers(Player* players, int* count);
};

#endif
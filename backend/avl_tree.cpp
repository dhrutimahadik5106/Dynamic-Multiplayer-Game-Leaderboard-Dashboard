#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

struct Node
{
    Player player;
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

AVLTree::AVLTree()
{
    root = 0;
}

int AVLTree::getHeight(Node* node)
{
    if(node == 0)
        return 0;
    return node->height;
}

int AVLTree::getBalance(Node* node)
{
    if(node == 0)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

Node* AVLTree::rotateRight(Node* y)
{
    Node* x = y->left;
    Node* T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    int h1 = getHeight(y->left);
    int h2 = getHeight(y->right);
    y->height = (h1 > h2 ? h1 : h2) + 1;
    
    h1 = getHeight(x->left);
    h2 = getHeight(x->right);
    x->height = (h1 > h2 ? h1 : h2) + 1;
    
    return x;
}

Node* AVLTree::rotateLeft(Node* x)
{
    Node* y = x->right;
    Node* T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    int h1 = getHeight(x->left);
    int h2 = getHeight(x->right);
    x->height = (h1 > h2 ? h1 : h2) + 1;
    
    h1 = getHeight(y->left);
    h2 = getHeight(y->right);
    y->height = (h1 > h2 ? h1 : h2) + 1;
    
    return y;
}

Node* AVLTree::insertNode(Node* node, Player player)
{
    if(node == 0)
    {
        Node* newNode = new Node;
        newNode->player = player;
        newNode->left = 0;
        newNode->right = 0;
        newNode->height = 1;
        return newNode;
    }
    
    if(player.score < node->player.score)
        node->left = insertNode(node->left, player);
    else if(player.score > node->player.score)
        node->right = insertNode(node->right, player);
    else
        return node;
    
    int h1 = getHeight(node->left);
    int h2 = getHeight(node->right);
    node->height = 1 + (h1 > h2 ? h1 : h2);
    
    int balance = getBalance(node);
    
    if(balance > 1 && player.score < node->left->player.score)
        return rotateRight(node);
    
    if(balance < -1 && player.score > node->right->player.score)
        return rotateLeft(node);
    
    if(balance > 1 && player.score > node->left->player.score)
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    
    if(balance < -1 && player.score < node->right->player.score)
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    
    return node;
}

Node* AVLTree::minValueNode(Node* node)
{
    Node* current = node;
    while(current->left != 0)
        current = current->left;
    return current;
}

void AVLTree::inorderWalk(Node* node, Player* players, int* index)
{
    if(node == 0)
        return;
    
    inorderWalk(node->right, players, index);
    players[*index] = node->player;
    (*index)++;
    inorderWalk(node->left, players, index);
}

void AVLTree::insert(Player player)
{
    root = insertNode(root, player);
}

void AVLTree::getSortedPlayers(Player* players, int* count)
{
    *count = 0;
    inorderWalk(root, players, count);
}


#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <vector>
#include <string>
//#include "player.h"
#include "player.h"



using namespace std;

/*-------------------------------------------------------
   AVL Tree Node
-------------------------------------------------------*/

struct Node
{
    Player player;
    Node* left;
    Node* right;
    int height;

    Node(Player p)
    {
        player = p;
        left = nullptr;
        right = nullptr;
        height = 1;
    }
};


/*-------------------------------------------------------
   AVL Tree Class
-------------------------------------------------------*/

class AVLTree
{

private:

    Node* root;

    int height(Node* node);

    int getBalance(Node* node);

    Node* rotateRight(Node* y);

    Node* rotateLeft(Node* x);

    Node* insertNode(Node* node, Player player);

    Node* deleteNode(Node* root, Player player);

    Node* minValueNode(Node* node);

    void inorder(Node* node, vector<Player>& players);


public:

    AVLTree();

    void insert(Player player);

    void remove(Player player);

    vector<Player> getSortedPlayers();

    int getHeight();
};

#endif

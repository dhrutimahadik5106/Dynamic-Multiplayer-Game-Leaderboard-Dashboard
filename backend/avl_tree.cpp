#include "avl_tree.h"
#include <algorithm>
#include <iostream>

using namespace std;


/*-------------------------------------------------------
    Constructor
-------------------------------------------------------*/

AVLTree::AVLTree()
{
    root = nullptr;
}


/*-------------------------------------------------------
    Get Height
-------------------------------------------------------*/

int AVLTree::height(Node* node)
{
    if(node == nullptr)
        return 0;

    return node->height;
}


/*-------------------------------------------------------
    Balance Factor
-------------------------------------------------------*/

int AVLTree::getBalance(Node* node)
{
    if(node == nullptr)
        return 0;

    return height(node->left) - height(node->right);
}


/*-------------------------------------------------------
    Right Rotation
-------------------------------------------------------*/

Node* AVLTree::rotateRight(Node* y)
{
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}


/*-------------------------------------------------------
    Left Rotation
-------------------------------------------------------*/

Node* AVLTree::rotateLeft(Node* x)
{
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}


/*-------------------------------------------------------
    Insert Player
-------------------------------------------------------*/

Node* AVLTree::insertNode(Node* node, Player player)
{
    if(node == nullptr)
        return new Node(player);

    if(player.score < node->player.score)
        node->left = insertNode(node->left, player);

    else if(player.score > node->player.score)
        node->right = insertNode(node->right, player);

    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);


    /* Left Left */
    if(balance > 1 && player.score < node->left->player.score)
        return rotateRight(node);

    /* Right Right */
    if(balance < -1 && player.score > node->right->player.score)
        return rotateLeft(node);

    /* Left Right */
    if(balance > 1 && player.score > node->left->player.score)
    {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }

    /* Right Left */
    if(balance < -1 && player.score < node->right->player.score)
    {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;
}


/*-------------------------------------------------------
    Find Minimum Node
-------------------------------------------------------*/

Node* AVLTree::minValueNode(Node* node)
{
    Node* current = node;

    while(current->left != nullptr)
        current = current->left;

    return current;
}


/*-------------------------------------------------------
    Delete Player
-------------------------------------------------------*/

Node* AVLTree::deleteNode(Node* root, int score)
{
    if(root == nullptr)
        return root;

    if(score < root->player.score)
        root->left = deleteNode(root->left, score);

    else if(score > root->player.score)
        root->right = deleteNode(root->right, score);

    else
    {
        if((root->left == nullptr) || (root->right == nullptr))
        {
            Node* temp = root->left ? root->left : root->right;

            if(temp == nullptr)
            {
                temp = root;
                root = nullptr;
            }
            else
                *root = *temp;

            delete temp;
        }
        else
        {
            Node* temp = minValueNode(root->right);

            root->player = temp->player;

            root->right = deleteNode(root->right, temp->player.score);
        }
    }

    if(root == nullptr)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    /* Left Left */
    if(balance > 1 && getBalance(root->left) >= 0)
        return rotateRight(root);

    /* Left Right */
    if(balance > 1 && getBalance(root->left) < 0)
    {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    /* Right Right */
    if(balance < -1 && getBalance(root->right) <= 0)
        return rotateLeft(root);

    /* Right Left */
    if(balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}


/*-------------------------------------------------------
    Public Insert
-------------------------------------------------------*/

void AVLTree::insert(Player player)
{
    root = insertNode(root, player);
}


/*-------------------------------------------------------
    Public Delete
-------------------------------------------------------*/

void AVLTree::remove(int score)
{
    root = deleteNode(root, score);
}


/*-------------------------------------------------------
    Inorder Traversal (Sorted Leaderboard)
-------------------------------------------------------*/

void AVLTree::inorder(Node* node, vector<Player>& players)
{
    if(node == nullptr)
        return;

    inorder(node->right, players);

    players.push_back(node->player);

    inorder(node->left, players);
}


/*-------------------------------------------------------
    Get Sorted Players
-------------------------------------------------------*/

vector<Player> AVLTree::getSortedPlayers()
{
    vector<Player> players;

    inorder(root, players);

    return players;
}

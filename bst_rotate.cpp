// bst_rotate.cpp - TV4: rotations, height, balance check, tree printing.
//
// The rotateX(Node*) functions return the NEW root of the rotated subtree; the caller
// must attach it to the parent (or to 'root'). In-order order never changes.
#include "BST.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

using namespace std;

// ------------------------------------------------------------ rotations (node level)

/* Left rotation: the right child y moves up.
 *
 *      x                y
 *       \              / \
 *        y     =>      x   c
 *       / \             \
 *      b   c             b
 */
Node* BST::rotateLeft(Node* x) {
    if (x == nullptr || x->right == nullptr) return x;   // cannot rotate: tree unchanged
    Node* y = x->right;
    x->right = y->left;
    y->left = x;
    return y;
}

// Right rotation: mirror image of rotateLeft.
Node* BST::rotateRight(Node* x) {
    if (x == nullptr || x->left == nullptr) return x;
    Node* y = x->left;
    x->left = y->right;
    y->right = x;
    return y;
}

// Left-Right: rotate the LEFT child to the left first, then rotate x to the right.
Node* BST::rotateLeftRight(Node* x) {
    if (x == nullptr || x->left == nullptr || x->left->right == nullptr) return x;
    x->left = rotateLeft(x->left);
    return rotateRight(x);
}

// Right-Left: rotate the RIGHT child to the right first, then rotate x to the left.
Node* BST::rotateRightLeft(Node* x) {
    if (x == nullptr || x->right == nullptr || x->right->left == nullptr) return x;
    x->right = rotateRight(x->right);
    return rotateLeft(x);
}

// ------------------------------------------------------------ rotation chosen by the user

// type: 1 = left, 2 = right, 3 = left-right, 4 = right-left.
// On failure prints the reason (English), leaves the tree unchanged and returns false.
bool BST::rotateAt(const string& id, int type) {
    if (root == nullptr) { cout << "Tree is empty." << endl; return false; }
    if (type < 1 || type > 4) { cout << "Invalid rotation type." << endl; return false; }

    // find the node and remember its parent
    Node* parent = nullptr;
    Node* node = root;
    while (node != nullptr && node->data.id != id) {
        parent = node;
        node = (id < node->data.id) ? node->left : node->right;
    }
    if (node == nullptr) {
        cout << "Cannot rotate: student ID " << id << " not found." << endl;
        return false;
    }

    // check the rotation is possible
    switch (type) {
        case 1:
            if (node->right == nullptr) { cout << "Cannot rotate left: node has no right child." << endl; return false; }
            break;
        case 2:
            if (node->left == nullptr) { cout << "Cannot rotate right: node has no left child." << endl; return false; }
            break;
        case 3:
            if (node->left == nullptr) { cout << "Cannot rotate left-right: node has no left child." << endl; return false; }
            if (node->left->right == nullptr) { cout << "Cannot rotate left-right: left child has no right child." << endl; return false; }
            break;
        case 4:
            if (node->right == nullptr) { cout << "Cannot rotate right-left: node has no right child." << endl; return false; }
            if (node->right->left == nullptr) { cout << "Cannot rotate right-left: right child has no left child." << endl; return false; }
            break;
    }

    Node* newTop = node;
    if (type == 1) newTop = rotateLeft(node);
    else if (type == 2) newTop = rotateRight(node);
    else if (type == 3) newTop = rotateLeftRight(node);
    else newTop = rotateRightLeft(node);

    // reconnect the rotated subtree
    if (parent == nullptr) root = newTop;                // rotated at the root: update root
    else if (parent->left == node) parent->left = newTop;
    else parent->right = newTop;
    return true;
}

// ------------------------------------------------------------ height and balance

// Height of a subtree: empty = 0, otherwise 1 + taller child.
int BST::height(Node* n) const {
    if (n == nullptr) return 0;
    return 1 + max(height(n->left), height(n->right));
}

int BST::height() const { return height(root); }

// Returns the height if every node below is balanced, otherwise -1.
int BST::checkBalance(Node* n) const {
    if (n == nullptr) return 0;
    int l = checkBalance(n->left);
    if (l < 0) return -1;
    int r = checkBalance(n->right);
    if (r < 0) return -1;
    if (abs(l - r) > 1) return -1;                       // children differ by more than 1
    return 1 + max(l, r);
}

// Balanced = EVERY node has two subtrees whose heights differ by at most 1.
bool BST::isBalanced() const { return checkBalance(root) != -1; }

// ------------------------------------------------------------ pretty print

// Right subtree first, indent = depth * 4 spaces. Tilt your head left to see the tree.
void BST::printTreeRec(Node* n, int depth) const {
    if (n == nullptr) return;
    printTreeRec(n->right, depth + 1);
    cout << string(static_cast<size_t>(depth) * 4, ' ') << n->data.id << '\n';
    printTreeRec(n->left, depth + 1);
}

void BST::printTree() const {
    if (root == nullptr) { cout << "Tree is empty." << endl; return; }
    printTreeRec(root, 0);
}

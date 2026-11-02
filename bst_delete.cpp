// bst_delete.cpp - TV3: deletion by Copying and by Merging.
//
// Both methods handle three cases: leaf, one child, two children.
//   Copying : two children -> copy the in-order PREDECESSOR (rightmost node of the
//             left subtree) into the node, then remove the predecessor node.
//   Merging : two children -> hang the right subtree under the rightmost node of the
//             left subtree, then replace the node by its left subtree.
#include "BST.h"

using namespace std;

// Finds the node with this ID and also returns its parent (nullptr when it is the root).
Node* BST::findWithParent(const string& id, Node*& parent) const {
    parent = nullptr;
    Node* cur = root;
    while (cur != nullptr) {
        if (id == cur->data.id) return cur;
        parent = cur;
        cur = (id < cur->data.id) ? cur->left : cur->right;
    }
    return nullptr;
}

// Makes 'parent' point to 'newChild' instead of 'oldChild'.
// If oldChild is the root (parent == nullptr) the root pointer itself changes.
void BST::replaceLink(Node* parent, Node* oldChild, Node* newChild) {
    if (parent == nullptr) root = newChild;
    else if (parent->left == oldChild) parent->left = newChild;
    else parent->right = newChild;
}

// ------------------------------------------------------------ Delete by Copying
bool BST::deleteByCopying(const string& id) {
    if (root == nullptr) return false;
    Node* parent = nullptr;
    Node* node = findWithParent(id, parent);
    if (node == nullptr) return false;               // ID does not exist

    if (node->left == nullptr && node->right == nullptr) {
        // Case 1: leaf -> parent forgets it
        replaceLink(parent, node, nullptr);
        delete node;
    } else if (node->left == nullptr || node->right == nullptr) {
        // Case 2: one child -> parent links directly to that child
        Node* child = (node->left != nullptr) ? node->left : node->right;
        replaceLink(parent, node, child);
        delete node;
    } else {
        // Case 3: two children -> find predecessor (rightmost node of the left subtree)
        Node* predParent = node;
        Node* pred = node->left;
        while (pred->right != nullptr) {
            predParent = pred;
            pred = pred->right;
        }
        node->data = pred->data;                     // COPY the predecessor's data
        // Remove the predecessor node: it has no right child, but may have a left child
        if (predParent == node) predParent->left = pred->left;
        else predParent->right = pred->left;
        delete pred;
    }
    return true;
}

// ------------------------------------------------------------ Delete by Merging
bool BST::deleteByMerging(const string& id) {
    if (root == nullptr) return false;
    Node* parent = nullptr;
    Node* node = findWithParent(id, parent);
    if (node == nullptr) return false;               // ID does not exist

    if (node->left == nullptr && node->right == nullptr) {
        // Case 1: leaf
        replaceLink(parent, node, nullptr);
    } else if (node->left == nullptr || node->right == nullptr) {
        // Case 2: one child
        Node* child = (node->left != nullptr) ? node->left : node->right;
        replaceLink(parent, node, child);
    } else {
        // Case 3: two children
        Node* leftSub = node->left;
        Node* rightSub = node->right;
        Node* rightmost = leftSub;                   // rightmost node of the left subtree
        while (rightmost->right != nullptr) rightmost = rightmost->right;
        rightmost->right = rightSub;                 // hang the whole right subtree there
        replaceLink(parent, node, leftSub);          // left subtree takes the node's place
    }
    delete node;                                     // free ONLY this node (subtrees stay)
    return true;
}

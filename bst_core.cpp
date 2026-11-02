// bst_core.cpp - TV2: constructor/destructor, insert, search, update, min/max, destroy.
#include "BST.h"

using namespace std;

BST::BST() : root(nullptr) {}

BST::~BST() { destroy(root); }

// Insert: walk down the tree comparing IDs (string order) until an empty spot is found.
// Returns false (and changes nothing) if the ID already exists.
bool BST::insert(const Student& s) {
    Node* parent = nullptr;
    Node* cur = root;
    while (cur != nullptr) {
        if (s.id == cur->data.id) return false;      // duplicate ID
        parent = cur;
        cur = (s.id < cur->data.id) ? cur->left : cur->right;
    }
    Node* fresh = new Node(s);                       // left = right = nullptr (see Node constructor)
    if (parent == nullptr) root = fresh;             // empty tree
    else if (s.id < parent->data.id) parent->left = fresh;
    else parent->right = fresh;
    return true;
}

// Search by ID: smaller -> go left, larger -> go right, equal -> found.
Node* BST::searchById(const string& id) const {
    Node* cur = root;
    while (cur != nullptr) {
        if (id == cur->data.id) return cur;
        cur = (id < cur->data.id) ? cur->left : cur->right;
    }
    return nullptr;                                  // not found (or empty tree)
}

// The tree is ordered by ID, so a name search must visit EVERY node.
// Matching ignores upper/lower case; results come out in ascending ID order.
void BST::collectByName(Node* n, const string& lowerName, vector<Node*>& out) const {
    if (n == nullptr) return;
    collectByName(n->left, lowerName, out);
    if (toLowerText(trimText(n->data.name)) == lowerName) out.push_back(n);
    collectByName(n->right, lowerName, out);
}

vector<Node*> BST::searchByName(const string& name) const {
    vector<Node*> result;
    collectByName(root, toLowerText(trimText(name)), result);
    return result;
}

// Update the student whose ID is 'id' with the values in 'info'.
// - Same ID: overwrite the fields.
// - New ID: check it is free FIRST, then delete the old node and insert the new one.
// Returns false (tree unchanged) if 'id' does not exist or the new ID is already used.
bool BST::update(const string& id, const Student& info) {
    Node* node = searchById(id);
    if (node == nullptr) return false;

    if (info.id == id) {
        node->data = info;
        return true;
    }
    if (searchById(info.id) != nullptr) return false;  // new ID already used: change nothing

    Student copy = info;                               // keep a copy before the node disappears
    if (!deleteByCopying(id)) return false;
    return insert(copy);                               // cannot fail: ID was free
}

// Smallest node of a subtree = go left as far as possible.
Node* BST::findMin(Node* n) {
    if (n == nullptr) return nullptr;
    while (n->left != nullptr) n = n->left;
    return n;
}

// Largest node of a subtree = go right as far as possible.
Node* BST::findMax(Node* n) {
    if (n == nullptr) return nullptr;
    while (n->right != nullptr) n = n->right;
    return n;
}

// Frees every node of a subtree (post-order: children first, then the node).
void BST::destroy(Node*& n) {
    if (n == nullptr) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
    n = nullptr;
}

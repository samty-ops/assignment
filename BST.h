// BST.h - class declaration shared by all members (owner: TV2).
// This is the "framework" agreed on day 3.
#ifndef BST_H
#define BST_H

#include <iosfwd>
#include <string>
#include <vector>
#include "Student.h"

struct Node {
    Student data;
    Node* left;
    Node* right;
    explicit Node(const Student& s) : data(s), left(nullptr), right(nullptr) {}
};

class BST {
private:
    Node* root;

    // ---- TV2 helpers ----
    void collectByName(Node* n, const std::string& lowerName, std::vector<Node*>& out) const;

    // ---- TV3 helpers ----
    Node* findWithParent(const std::string& id, Node*& parent) const;
    void replaceLink(Node* parent, Node* oldChild, Node* newChild);

    // ---- TV4 helpers ----
    int checkBalance(Node* n) const;  // returns -1 if unbalanced, otherwise the height
    void printTreeRec(Node* n, int depth) const;

    // ---- TV5 helpers ----
    void inorder(Node* n) const;
    void preorder(Node* n) const;
    void postorder(Node* n) const;
    void collectInorder(Node* n, std::vector<const Node*>& out) const;
    void savePreorder(Node* n, std::ostream& out) const;

public:
    BST();
    ~BST();
    BST(const BST&) = delete;             // a tree owns its nodes: no copying
    BST& operator=(const BST&) = delete;

    // ---- TV2: core (bst_core.cpp) ----
    bool insert(const Student& s);
    Node* searchById(const std::string& id) const;
    std::vector<Node*> searchByName(const std::string& name) const;
    bool update(const std::string& id, const Student& info);
    Node* findMin(Node* n);
    Node* findMax(Node* n);
    void destroy(Node*& n);

    // ---- TV3: deletion (bst_delete.cpp) ----
    bool deleteByCopying(const std::string& id);
    bool deleteByMerging(const std::string& id);

    // ---- TV4: rotation / balance (bst_rotate.cpp) ----
    Node* rotateLeft(Node* x);
    Node* rotateRight(Node* x);
    Node* rotateLeftRight(Node* x);
    Node* rotateRightLeft(Node* x);
    bool rotateAt(const std::string& id, int type);  // 1=L 2=R 3=LR 4=RL
    int height(Node* n) const;
    int height() const;                              // height of the whole tree
    bool isBalanced() const;
    void printTree() const;

    // ---- TV5: traversal, statistics, file (bst_report.cpp) ----
    void inorder() const;
    void preorder() const;
    void postorder() const;
    void levelOrder() const;
    void listScholarship(float minGpa = 3.2f) const;
    void showStatistics() const;
    void displayByMajor(const std::string& major) const;
    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);
};

#endif

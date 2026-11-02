// main.cpp - TV1: menu and main().
// The BST functions never read the keyboard: this file asks the user for data,
// calls the BST, and prints the result message.
#include <iostream>
#include <string>
#include <vector>

#include "BST.h"
#include "Student.h"

using namespace std;

static const int EXIT_CHOICE = 0;
static const int INVALID_CHOICE = -1;
static const int INPUT_CLOSED = -2;

static void printMenu() {
    cout << "\n===== STUDENT MANAGEMENT (BST) =====\n"
         << " 1. Add new student\n"
         << " 2. Delete student (by Copying)\n"
         << " 3. Delete student (by Merging)\n"
         << " 4. Search student by ID\n"
         << " 5. Search student by name\n"
         << " 6. Update student\n"
         << " 7. Display in-order (LNR)\n"
         << " 8. Display pre-order (NLR)\n"
         << " 9. Display post-order (LRN)\n"
         << "10. Display level-order (BFS)\n"
         << "11. Rotate left at a node\n"
         << "12. Rotate right at a node\n"
         << "13. Rotate left-right at a node\n"
         << "14. Rotate right-left at a node\n"
         << "15. Display students by major\n"
         << "16. List scholarship students (GPA >= 3.2)\n"
         << "17. Show statistics\n"
         << "--- Bonus ---\n"
         << "18. Save tree to file\n"
         << "19. Load tree from file\n"
         << "20. Show tree structure\n"
         << "21. Show tree height and balance\n"
         << " 0. Exit\n";
}

// Reads the menu choice. Returns INVALID_CHOICE for text / out-of-range numbers.
static int readChoice() {
    string line;
    if (!readLineSafe("Enter your choice: ", line)) return INPUT_CLOSED;
    if (line.empty()) return INVALID_CHOICE;
    for (unsigned char c : line)
        if (!isdigit(c)) return INVALID_CHOICE;
    if (line.size() > 2) return INVALID_CHOICE;
    int v = stoi(line);
    return (v >= 0 && v <= 21) ? v : INVALID_CHOICE;
}

static bool askId(const string& prompt, string& id) {
    if (!readLineSafe(prompt, id)) return false;
    if (id.empty()) { cout << "ID must not be empty." << endl; return false; }
    return true;
}

static void doAdd(BST& tree) {
    Student s;
    if (!readStudent(s)) { cout << "Input cancelled." << endl; return; }
    if (tree.insert(s)) cout << "Student " << s.id << " added." << endl;
    else cout << "Cannot add: ID " << s.id << " already exists." << endl;
}

static void doDelete(BST& tree, bool byCopying) {
    string id;
    if (!askId("Enter student ID to delete: ", id)) return;
    bool ok = byCopying ? tree.deleteByCopying(id) : tree.deleteByMerging(id);
    if (ok) cout << "Student " << id << " deleted (" << (byCopying ? "Copying" : "Merging") << ")." << endl;
    else cout << "Cannot delete: student ID " << id << " not found (or tree is empty)." << endl;
}

static void doSearchId(const BST& tree) {
    string id;
    if (!askId("Enter student ID to search: ", id)) return;
    Node* n = tree.searchById(id);
    if (n == nullptr) { cout << "Student ID " << id << " not found." << endl; return; }
    printStudentHeader();
    printStudent(n->data);
}

static void doSearchName(const BST& tree) {
    string name;
    if (!readLineSafe("Enter full name to search: ", name)) return;
    vector<Node*> found = tree.searchByName(name);
    if (found.empty()) { cout << "No student found with name: " << name << endl; return; }
    printStudentHeader();
    for (const Node* n : found) printStudent(n->data);
    cout << "Found " << found.size() << " student(s)." << endl;
}

static void doUpdate(BST& tree) {
    string id;
    if (!askId("Enter student ID to update: ", id)) return;
    Node* n = tree.searchById(id);
    if (n == nullptr) { cout << "Student ID " << id << " not found." << endl; return; }

    cout << "Current record:" << endl;
    printStudentHeader();
    printStudent(n->data);
    cout << "Press Enter to keep the current value of a field." << endl;

    Student updated;
    if (!readStudentUpdate(n->data, updated)) { cout << "Input cancelled." << endl; return; }
    if (tree.update(id, updated)) cout << "Student updated." << endl;
    else cout << "Update failed: the new ID already exists. Nothing was changed." << endl;
}

static void doRotate(BST& tree, int type) {
    string id;
    if (!askId("Enter ID of the node to rotate at: ", id)) return;
    if (tree.rotateAt(id, type)) {
        cout << "Rotation completed. Tree now (tilt your head left):" << endl;
        tree.printTree();
    }
}

static void doByMajor(const BST& tree) {
    string major;
    if (!readLineSafe("Enter major (leave empty to show all, grouped by major): ", major)) return;
    tree.displayByMajor(major);
}

static void doSave(const BST& tree) {
    string path;
    if (!readLineSafe("Enter file name to save (empty = students.txt): ", path)) return;
    if (path.empty()) path = "students.txt";
    if (tree.saveToFile(path)) cout << "Tree saved to " << path << "." << endl;
    else cout << "Cannot write file: " << path << endl;
}

static void doLoad(BST& tree) {
    string path;
    if (!readLineSafe("Enter file name to load (empty = students.txt): ", path)) return;
    if (path.empty()) path = "students.txt";
    if (!tree.loadFromFile(path)) cout << "Cannot open file: " << path << " (tree unchanged)." << endl;
}

static void doHeight(const BST& tree) {
    cout << "Tree height: " << tree.height() << endl;
    cout << "Balanced: " << (tree.isBalanced() ? "yes" : "no") << endl;
}

// Main loop: show menu -> read choice -> do the work -> show result -> repeat.
void runMenu(BST& tree) {
    while (true) {
        printMenu();
        int choice = readChoice();
        if (choice == INPUT_CLOSED || choice == EXIT_CHOICE) break;
        switch (choice) {
            case 1:  doAdd(tree); break;
            case 2:  doDelete(tree, true); break;
            case 3:  doDelete(tree, false); break;
            case 4:  doSearchId(tree); break;
            case 5:  doSearchName(tree); break;
            case 6:  doUpdate(tree); break;
            case 7:  tree.inorder(); break;
            case 8:  tree.preorder(); break;
            case 9:  tree.postorder(); break;
            case 10: tree.levelOrder(); break;
            case 11: doRotate(tree, 1); break;
            case 12: doRotate(tree, 2); break;
            case 13: doRotate(tree, 3); break;
            case 14: doRotate(tree, 4); break;
            case 15: doByMajor(tree); break;
            case 16: tree.listScholarship(); break;
            case 17: tree.showStatistics(); break;
            case 18: doSave(tree); break;
            case 19: doLoad(tree); break;
            case 20: tree.printTree(); break;
            case 21: doHeight(tree); break;
            default: cout << "Invalid choice. Please enter a number from 0 to 21." << endl;
        }
    }
    cout << "Goodbye." << endl;
}

int main() {
    BST tree;          // the destructor frees every node when main ends
    runMenu(tree);
    return 0;
}

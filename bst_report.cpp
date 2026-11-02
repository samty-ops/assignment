// bst_report.cpp - TV5: traversals, scholarship, statistics, display by major,
// save/load file.
//
// Algorithms follow sheet "Thiết kế", section 4 (items 12, 13, 14).

#include "BST.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {

// Lower-case copy of a string (used to compare majors case-insensitively).
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}

// Removes spaces / tabs / '\r' at both ends.
string trim(const string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

void printEmptyMessage() { cout << "Tree is empty." << endl; }

// Used by statistics: one counter per major.
struct MajorCount {
    string key;    // lower-case major, used for comparing
    string label;  // major as first seen, used for printing
    int count;
};

// Finds the counter of a major, or returns -1.
int findMajor(const vector<MajorCount>& list, const string& key) {
    for (size_t i = 0; i < list.size(); ++i)
        if (list[i].key == key) return static_cast<int>(i);
    return -1;
}

// '|' is the field separator in the save file, so it must not appear in text.
string cleanField(string s) {
    replace(s.begin(), s.end(), '|', '/');
    replace(s.begin(), s.end(), '\n', ' ');
    replace(s.begin(), s.end(), '\r', ' ');
    return s;
}

// Splits "a|b|c" into {"a","b","c"}.
vector<string> splitBar(const string& line) {
    vector<string> parts;
    string cur;
    for (char c : line) {
        if (c == '|') { parts.push_back(cur); cur.clear(); }
        else cur += c;
    }
    parts.push_back(cur);
    return parts;
}

// Reads one line of the save file into a Student and validates it.
// Returns false and sets 'err' (English message) when the line is bad.
bool parseLine(const string& line, Student& s, string& err) {
    vector<string> f = splitBar(line);
    if (f.size() != 6) { err = "expected 6 fields separated by '|'"; return false; }
    for (string& x : f) x = trim(x);

    if (f[0].empty() || f[0].find(' ') != string::npos) { err = "invalid student ID"; return false; }
    if (f[1].empty()) { err = "empty full name"; return false; }
    if (!isValidDate(f[2])) { err = "invalid date of birth (need YYYY-MM-DD)"; return false; }
    if (f[3].empty()) { err = "empty major"; return false; }

    double gpa = 0;
    int year = 0;
    try {
        size_t p1 = 0, p2 = 0;
        gpa = stod(f[4], &p1);
        year = stoi(f[5], &p2);
        if (p1 != f[4].size() || p2 != f[5].size()) { err = "GPA or year is not a number"; return false; }
    } catch (...) {
        err = "GPA or year is not a number";
        return false;
    }
    if (gpa < 0.0 || gpa > 4.0) { err = "GPA must be between 0.0 and 4.0"; return false; }
    if (year < MIN_ENROLL_YEAR || year > MAX_ENROLL_YEAR) { err = "enrollment year out of range"; return false; }

    s.id = f[0]; s.name = f[1]; s.dob = f[2]; s.major = f[3];
    s.gpa = static_cast<float>(gpa); s.year = year;
    return true;
}

}  // namespace

// ======================= Traversals =======================

// In-order (LNR): left -> node -> right. Prints IDs in ascending order.
void BST::inorder(Node* n) const {
    if (n == nullptr) return;
    inorder(n->left);
    printStudent(n->data);
    inorder(n->right);
}

// Pre-order (NLR): node -> left -> right.
void BST::preorder(Node* n) const {
    if (n == nullptr) return;
    printStudent(n->data);
    preorder(n->left);
    preorder(n->right);
}

// Post-order (LRN): left -> right -> node.
void BST::postorder(Node* n) const {
    if (n == nullptr) return;
    postorder(n->left);
    postorder(n->right);
    printStudent(n->data);
}

void BST::inorder() const {
    if (root == nullptr) { printEmptyMessage(); return; }
    cout << "=== In-order (LNR) ===" << endl;
    printStudentHeader();
    inorder(root);
}

void BST::preorder() const {
    if (root == nullptr) { printEmptyMessage(); return; }
    cout << "=== Pre-order (NLR) ===" << endl;
    printStudentHeader();
    preorder(root);
}

void BST::postorder() const {
    if (root == nullptr) { printEmptyMessage(); return; }
    cout << "=== Post-order (LRN) ===" << endl;
    printStudentHeader();
    postorder(root);
}

// Level-order (BFS): uses a queue, NOT recursion.
void BST::levelOrder() const {
    if (root == nullptr) { printEmptyMessage(); return; }
    cout << "=== Level-order (BFS) ===" << endl;
    printStudentHeader();

    queue<const Node*> q;
    q.push(root);
    while (!q.empty()) {
        const Node* cur = q.front();
        q.pop();
        printStudent(cur->data);
        if (cur->left != nullptr) q.push(cur->left);    // left child first
        if (cur->right != nullptr) q.push(cur->right);  // then right child
    }
}

// Collects all nodes in in-order (so the result is sorted by ID).
void BST::collectInorder(Node* n, vector<const Node*>& out) const {
    if (n == nullptr) return;
    collectInorder(n->left, out);
    out.push_back(n);
    collectInorder(n->right, out);
}

// ======================= Scholarship =======================

// Prints students whose GPA >= minGpa (default 3.2), in ascending ID order.
void BST::listScholarship(float minGpa) const {
    if (root == nullptr) { printEmptyMessage(); return; }

    vector<const Node*> all;
    collectInorder(root, all);

    vector<const Node*> eligible;
    for (const Node* n : all)
        if (n->data.gpa + 1e-6f >= minGpa) eligible.push_back(n);  // small epsilon for float compare

    if (eligible.empty()) {
        cout << "No scholarship-eligible students (GPA >= " << minGpa << ")." << endl;
        return;
    }
    cout << "=== Scholarship-eligible students (GPA >= " << minGpa << ") ===" << endl;
    printStudentHeader();
    for (const Node* n : eligible) printStudent(n->data);
    cout << "Total: " << eligible.size() << " student(s)." << endl;
}

// ======================= Statistics =======================

// Total students, students per major, students with GPA >= 3.2.
void BST::showStatistics() const {
    int total = 0;
    int eligible = 0;
    vector<MajorCount> majors;

    vector<const Node*> all;
    collectInorder(root, all);  // empty tree -> empty list -> all numbers stay 0

    for (const Node* n : all) {
        ++total;
        if (n->data.gpa + 1e-6f >= 3.2f) ++eligible;

        string key = toLower(n->data.major);
        int idx = findMajor(majors, key);
        if (idx >= 0) ++majors[idx].count;
        else majors.push_back({key, n->data.major, 1});
    }

    cout << "=== Statistics ===" << endl;
    cout << "Total students: " << total << endl;
    cout << "Students per major:" << endl;
    if (majors.empty()) cout << "  (none)" << endl;
    for (const MajorCount& m : majors)
        cout << "  " << m.label << ": " << m.count << endl;
    cout << "Students with GPA >= 3.2: " << eligible << endl;
}

// ======================= Display by major =======================

// major != "": print students of that major (case-insensitive).
// major == "": print all students grouped by major.
void BST::displayByMajor(const string& major) const {
    if (root == nullptr) { printEmptyMessage(); return; }

    vector<const Node*> all;
    collectInorder(root, all);
    string want = toLower(trim(major));

    if (!want.empty()) {
        vector<const Node*> found;
        for (const Node* n : all)
            if (toLower(n->data.major) == want) found.push_back(n);

        if (found.empty()) {
            cout << "No students found in major: " << major << endl;
            return;
        }
        cout << "=== Students in major " << found[0]->data.major << " ===" << endl;
        printStudentHeader();
        for (const Node* n : found) printStudent(n->data);
        cout << "Total: " << found.size() << " student(s)." << endl;
        return;
    }

    // Group by major, in order of first appearance.
    vector<MajorCount> groups;
    for (const Node* n : all) {
        string key = toLower(n->data.major);
        int idx = findMajor(groups, key);
        if (idx >= 0) ++groups[idx].count;
        else groups.push_back({key, n->data.major, 1});
    }
    for (const MajorCount& g : groups) {
        cout << "=== Major: " << g.label << " (" << g.count << " student(s)) ===" << endl;
        printStudentHeader();
        for (const Node* n : all)
            if (toLower(n->data.major) == g.key) printStudent(n->data);
        cout << endl;
    }
}

// ======================= Save / Load =======================

// Writes one student per line in PRE-ORDER, so loading the file (inserting
// line by line) rebuilds the SAME tree shape.
// Line format: id|name|dob|major|gpa|year
void BST::savePreorder(Node* n, ostream& out) const {
    if (n == nullptr) return;
    const Student& s = n->data;
    out << cleanField(s.id) << '|' << cleanField(s.name) << '|' << cleanField(s.dob) << '|'
        << cleanField(s.major) << '|' << s.gpa << '|' << s.year << '\n';
    savePreorder(n->left, out);
    savePreorder(n->right, out);
}

// Returns false if the file cannot be written.
bool BST::saveToFile(const string& path) const {
    ofstream out(path);
    if (!out) return false;
    savePreorder(root, out);
    out.flush();
    return out.good();
}

// Replaces the current tree with the one stored in the file.
// - File cannot be opened: returns false and the current tree is NOT changed.
// - Bad lines (missing fields, invalid values, duplicate ID) are reported and skipped.
bool BST::loadFromFile(const string& path) {
    ifstream in(path);
    if (!in) return false;

    vector<Student> records;
    string line;
    int lineNo = 0;
    int skipped = 0;
    while (getline(in, line)) {
        ++lineNo;
        if (trim(line).empty()) continue;  // ignore blank lines
        Student s;
        string err;
        if (!parseLine(line, s, err)) {
            cout << "Line " << lineNo << " skipped: " << err << endl;
            ++skipped;
            continue;
        }
        records.push_back(s);
    }

    destroy(root);   // free the old tree
    root = nullptr;

    int loaded = 0;
    for (const Student& s : records) {
        if (insert(s)) ++loaded;
        else {
            cout << "Student " << s.id << " skipped: duplicate ID" << endl;
            ++skipped;
        }
    }
    cout << "Loaded " << loaded << " student(s), skipped " << skipped << " line(s)." << endl;
    return true;
}

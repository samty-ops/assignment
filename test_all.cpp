// test_all.cpp - automatic tests for TV1 (validation), TV2 (core), TV3 (delete), TV4 (rotation).
// Build and run:  make test
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "BST.h"

using namespace std;

static int passed = 0, failed = 0;
static void check(bool ok, const string& name) {
    if (ok) ++passed; else ++failed;
    cout << (ok ? "[PASS] " : "[FAIL] ") << name << endl;
}

template <class F>
static string capture(F f) {
    ostringstream buf;
    streambuf* old = cout.rdbuf(buf.rdbuf());
    f();
    cout.rdbuf(old);
    return buf.str();
}

// IDs ("S" + 3 digits at the start of a line), in the order printed.
static string ids(const string& text) {
    string r;
    size_t pos = 0;
    while (pos < text.size()) {
        size_t eol = text.find('\n', pos);
        if (eol == string::npos) eol = text.size();
        string line = text.substr(pos, eol - pos);
        size_t st = line.find_first_not_of(' ');
        if (st != string::npos && line.size() >= st + 4 && line[st] == 'S' && isdigit((unsigned char)line[st + 1]) &&
            isdigit((unsigned char)line[st + 2]) && isdigit((unsigned char)line[st + 3]))
            r += line.substr(st + 1, 3) + " ";
        pos = eol + 1;
    }
    return r;
}
static string pre(const BST& t)  { return ids(capture([&] { t.preorder(); })); }
static string in(const BST& t)   { return ids(capture([&] { t.inorder(); })); }

static Student mk(const string& id, const string& name = "Name", float gpa = 3.0f, const string& major = "IT") {
    return Student{id, name, "2005-01-10", major, gpa, 2023};
}
static void add(BST& t, initializer_list<const char*> list) {
    for (const char* id : list) t.insert(mk(id));
}

// Tree used in the design sheet:  50 (30 (20, 40 (35, 45)), 70)
static void buildDesignTree(BST& t) { add(t, {"S050", "S030", "S070", "S020", "S040", "S035", "S045"}); }

int main() {
    // ================= TV1: validation =================
    check(isValidDate("2005-01-10"), "TV1 date: valid");
    check(isValidDate("2004-02-29"), "TV1 date: leap day 2004");
    check(!isValidDate("2005-02-29"), "TV1 date: 29 Feb 2005 rejected");
    check(!isValidDate("2005-02-30"), "TV1 date: 30 Feb rejected");
    check(!isValidDate("2005-13-01"), "TV1 date: month 13 rejected");
    check(!isValidDate("2005-1-10") && !isValidDate("10-01-2005") && !isValidDate("abcd-ef-gh"), "TV1 date: wrong format rejected");
    float g = -1; int y = 0;
    check(parseGpa("3.5", g) && g > 3.49f && g < 3.51f, "TV1 gpa: 3.5");
    check(parseGpa("0", g) && parseGpa("4.0", g), "TV1 gpa: 0 and 4.0 accepted");
    check(!parseGpa("4.1", g) && !parseGpa("-0.1", g) && !parseGpa("abc", g) && !parseGpa("3.5x", g) && !parseGpa("nan", g) && !parseGpa("", g), "TV1 gpa: bad values rejected");
    check(parseYear("2023", y) && y == 2023, "TV1 year: 2023");
    check(!parseYear("1999", y) && !parseYear("2027", y) && !parseYear("20x3", y) && !parseYear("", y), "TV1 year: bad values rejected");
    check(isValidId("S030") && !isValidId("") && !isValidId("S 030"), "TV1 id: empty / spaces rejected");

    // ================= TV2: core =================
    {
        BST t;
        check(t.insert(mk("S030")) && t.insert(mk("S010")) && t.insert(mk("S050")), "TV2 insert: 3 students");
        check(in(t) == "010 030 050 ", "TV2 insert: in-order ascending");
        check(!t.insert(mk("S030", "Other")), "TV2 insert: duplicate ID rejected");
        check(in(t) == "010 030 050 ", "TV2 insert: tree unchanged after duplicate");
        check(t.searchById("S050") != nullptr && t.searchById("S050")->data.id == "S050", "TV2 search ID: found");
        check(t.searchById("S999") == nullptr, "TV2 search ID: not found");
        BST e;
        check(e.searchById("S001") == nullptr, "TV2 search ID: empty tree");
        check(t.findMin(t.searchById("S030"))->data.id == "S010" && t.findMax(t.searchById("S030"))->data.id == "S050", "TV2 findMin / findMax");
        check(t.findMin(nullptr) == nullptr, "TV2 findMin(nullptr)");
    }
    {
        BST t;
        t.insert(mk("S001", "An Nguyen"));
        t.insert(mk("S002", "AN NGUYEN"));
        t.insert(mk("S003", "Binh"));
        check(t.searchByName("an nguyen").size() == 2, "TV2 search name: case-insensitive, 2 results");
        check(t.searchByName("an nguyen")[0]->data.id == "S001", "TV2 search name: ascending ID order");
        check(t.searchByName("  Binh ").size() == 1, "TV2 search name: spaces ignored");
        check(t.searchByName("Zed").empty(), "TV2 search name: not found");
    }
    {
        BST t;
        add(t, {"S030", "S010", "S050"});
        Student u = mk("S010", "New Name", 3.9f);
        check(t.update("S010", u) && t.searchById("S010")->data.name == "New Name", "TV2 update: same ID, fields changed");
        check(in(t) == "010 030 050 ", "TV2 update: shape unchanged");
        check(t.update("S010", mk("S020")) && t.searchById("S010") == nullptr && t.searchById("S020") != nullptr, "TV2 update: ID changed");
        check(in(t) == "020 030 050 ", "TV2 update: in-order still ascending");
        check(!t.update("S020", mk("S050")), "TV2 update: new ID already used -> false");
        check(in(t) == "020 030 050 " && t.searchById("S020") != nullptr, "TV2 update: nothing lost after failed ID change");
        check(!t.update("S999", mk("S999")), "TV2 update: unknown ID -> false");
    }

    // ================= TV3: deletion =================
    for (int method = 0; method < 2; ++method) {
        string m = method == 0 ? "copying" : "merging";
        auto del = [&](BST& t, const char* id) { return method == 0 ? t.deleteByCopying(id) : t.deleteByMerging(id); };
        {   // leaf
            BST t; buildDesignTree(t);
            check(del(t, "S035") && pre(t) == "050 030 020 040 045 070 ", "TV3 " + m + ": delete leaf");
        }
        {   // one child (S040 has two children here, so build a dedicated tree)
            BST t; add(t, {"S050", "S030", "S070", "S020", "S040", "S035"});
            check(del(t, "S040") && pre(t) == "050 030 020 035 070 " && in(t) == "020 030 035 050 070 ", "TV3 " + m + ": delete node with one child");
        }
        {   // one child, right side
            BST t; add(t, {"S050", "S030", "S070", "S080"});
            check(del(t, "S070") && pre(t) == "050 030 080 ", "TV3 " + m + ": delete node with one (right) child");
        }
        {   // two children, non-root
            BST t; buildDesignTree(t);
            check(del(t, "S030") && pre(t) == "050 020 040 035 045 070 " && in(t) == "020 035 040 045 050 070 ", "TV3 " + m + ": delete node with two children");
        }
        {   // two children, root
            BST t; buildDesignTree(t);
            check(del(t, "S050") && in(t) == "020 030 035 040 045 070 ", "TV3 " + m + ": delete root (two children): in-order ascending");
            check(t.searchById("S050") == nullptr, "TV3 " + m + ": deleted ID is gone");
        }
        {   // root with one child / only node
            BST t; add(t, {"S050", "S030"});
            check(del(t, "S050") && pre(t) == "030 ", "TV3 " + m + ": delete root with one child");
            check(del(t, "S030") && in(t).empty(), "TV3 " + m + ": delete the only node");
            check(!del(t, "S030"), "TV3 " + m + ": delete on empty tree -> false");
            check(t.insert(mk("S001")) && in(t) == "001 ", "TV3 " + m + ": tree usable after becoming empty");
        }
        {   // unknown ID
            BST t; buildDesignTree(t);
            check(!del(t, "S999") && in(t) == "020 030 035 040 045 050 070 ", "TV3 " + m + ": unknown ID -> false, tree unchanged");
        }
        {   // delete everything one by one
            BST t; buildDesignTree(t);
            bool ok = true;
            for (const char* id : {"S030", "S050", "S045", "S020", "S070", "S040", "S035"}) ok = del(t, id) && ok;
            check(ok && in(t).empty(), "TV3 " + m + ": delete every node one by one");
        }
    }
    {   // the two methods give different shapes on purpose (see design sheet)
        BST a, b; buildDesignTree(a); buildDesignTree(b);
        a.deleteByCopying("S050"); b.deleteByMerging("S050");
        check(pre(a) == "045 030 020 040 035 070 ", "TV3 copying: design example shape");
        check(pre(b) == "030 020 040 035 045 070 ", "TV3 merging: design example shape");
        check(pre(a) != pre(b), "TV3 the two methods give different shapes on the design example");
        // Merging can make the tree taller: left subtree leans right, right subtree is a long chain.
        BST c, d; add(c, {"S050", "S030", "S035", "S038", "S070", "S080", "S090"}); add(d, {"S050", "S030", "S035", "S038", "S070", "S080", "S090"});
        c.deleteByCopying("S050"); d.deleteByMerging("S050");
        check(c.height() == 4 && d.height() == 6 && in(c) == in(d), "TV3 merging can be taller (6) than copying (4), same in-order");
    }

    // ================= TV4: rotation =================
    {   // left rotation on a right-skewed chain
        BST t; add(t, {"S010", "S020", "S030"});
        check(t.height() == 3 && !t.isBalanced(), "TV4: skewed chain has height 3, not balanced");
        check(t.rotateAt("S010", 1) && pre(t) == "020 010 030 " && in(t) == "010 020 030 ", "TV4: rotate left at root");
        check(t.height() == 2 && t.isBalanced(), "TV4: after rotation height 2, balanced");
    }
    {
        BST t; add(t, {"S030", "S020", "S010"});
        check(t.rotateAt("S030", 2) && pre(t) == "020 010 030 ", "TV4: rotate right at root");
    }
    {
        BST t; add(t, {"S030", "S010", "S020"});
        check(t.rotateAt("S030", 3) && pre(t) == "020 010 030 " && in(t) == "010 020 030 ", "TV4: rotate left-right");
    }
    {
        BST t; add(t, {"S010", "S030", "S020"});
        check(t.rotateAt("S010", 4) && pre(t) == "020 010 030 " && in(t) == "010 020 030 ", "TV4: rotate right-left");
    }
    {   // rotation in the middle of the tree: parent must be reconnected
        BST t; add(t, {"S050", "S060", "S070", "S080"});
        check(t.rotateAt("S060", 1) && pre(t) == "050 070 060 080 " && in(t) == "050 060 070 080 ", "TV4: rotate left below the root (parent reconnected)");
    }
    {   // invalid rotations leave the tree unchanged and print a reason
        BST t; add(t, {"S030", "S010", "S050"});
        string before = pre(t);
        bool r1 = true, r2 = true, r3 = true, r4 = true, r5 = true, r6 = true;
        string m1 = capture([&] { r1 = t.rotateAt("S010", 1); });
        capture([&] { r2 = t.rotateAt("S010", 2); });
        capture([&] { r3 = t.rotateAt("S030", 3); });
        capture([&] { r4 = t.rotateAt("S030", 4); });
        string m5 = capture([&] { r5 = t.rotateAt("S999", 1); });
        capture([&] { r6 = t.rotateAt("S030", 9); });
        check(!r1 && !r2 && !r3 && !r4 && !r5 && !r6, "TV4: invalid rotations return false");
        check(pre(t) == before, "TV4: tree unchanged after invalid rotations");
        check(m1.find("no right child") != string::npos && m5.find("not found") != string::npos, "TV4: reasons are printed");
        BST e;
        check(!e.rotateAt("S001", 1), "TV4: rotate on empty tree -> false");
    }
    {   // balance and height
        BST e;
        check(e.height() == 0 && e.isBalanced(), "TV4: empty tree height 0, balanced");
        BST t; add(t, {"S030", "S010", "S050"});
        check(t.height() == 2 && t.isBalanced(), "TV4: 3-node tree height 2, balanced");
        BST u; add(u, {"S050", "S030", "S070", "S020", "S010"});
        check(u.height() == 4 && !u.isBalanced(), "TV4: deep left side -> not balanced (checks every node, not only root)");
    }
    {   // printTree format: right subtree first, indent 4 per level
        BST t; add(t, {"S030", "S010", "S050"});
        check(capture([&] { t.printTree(); }) == "    S050\nS030\n    S010\n", "TV4: printTree format");
        BST e;
        check(capture([&] { e.printTree(); }).find("Tree is empty") != string::npos, "TV4: printTree on empty tree");
    }

    cout << "\nResult: " << passed << " passed, " << failed << " failed." << endl;
    return failed == 0 ? 0 : 1;
}

# CSD202 – Topic 4: Student Management with a Binary Search Tree (C++17)

A console program that stores students in a hand-written Binary Search Tree (BST), ordered by Student ID.
No `std::map` / `std::set` is used for the tree.

## Build and run

```
make                 # builds ./student_mgmt   (g++ -std=c++17 -Wall -Wextra)
./student_mgmt       # starts the menu (0 = exit, memory is freed on exit)
make test            # builds and runs all automatic tests
make asan            # same tests with AddressSanitizer (memory-leak check)
```

Without `make`:

```
g++ -std=c++17 -Wall -Wextra -Isrc src/bst_core.cpp src/bst_delete.cpp src/bst_rotate.cpp src/bst_report.cpp src/main.cpp -o student_mgmt
```

Run the scripted demo session:

```
./student_mgmt < INPUT/sample_menu_input.txt > OUTPUT/my_output.txt
```

## Menu

| # | Function | # | Function |
|---|----------|---|----------|
| 1 | Add student | 11 | Rotate left at a node |
| 2 | Delete (Copying) | 12 | Rotate right at a node |
| 3 | Delete (Merging) | 13 | Rotate left-right at a node |
| 4 | Search by ID | 14 | Rotate right-left at a node |
| 5 | Search by name | 15 | Display by major |
| 6 | Update (Enter keeps a value; the ID may change) | 16 | Scholarship list (GPA >= 3.2) |
| 7 | In-order (LNR) | 17 | Statistics |
| 8 | Pre-order (NLR) | 18 | Save to file (bonus) |
| 9 | Post-order (LRN) | 19 | Load from file (bonus) |
| 10 | Level-order (BFS) | 20 / 21 | Tree structure / height and balance (bonus) |

## Source files and owners

| File | Content | Owner |
|------|---------|-------|
| `src/Student.h` | `Student`, validation, safe input | TV1 |
| `src/main.cpp` | menu and `main` | TV1 |
| `src/BST.h` | `Node` and `class BST` (the agreed "framework") | TV2 |
| `src/bst_core.cpp` | insert, search by ID/name, update, min/max, destroy | TV2 |
| `src/bst_delete.cpp` | delete by Copying and by Merging | TV3 |
| `src/bst_rotate.cpp` | 4 rotations, height, balance check, tree printing | TV4 |
| `src/bst_report.cpp` | 4 traversals, scholarship, statistics, by major, save/load | TV5 |
| `tests/test_all.cpp` | tests for TV1–TV4 | all |
| `tests/test_tv5.cpp` | tests for TV5 | TV5 |

## Design decisions

* Student data: `id|name|dob(YYYY-MM-DD)|major|gpa(0.0-4.0)|year(2000-2026)`. IDs are compared as strings, so use IDs of the same length (e.g. `S030`).
* Delete by Copying: copy the in-order predecessor (rightmost node of the left subtree) into the node, then remove the predecessor.
* Delete by Merging: hang the right subtree under the rightmost node of the left subtree; the left subtree replaces the node.
* Rotations return the new root of the subtree; `rotateAt` reconnects it to the parent (or to `root`) and prints the reason when a rotation is impossible.
* Name search ignores upper/lower case and returns every match. Major comparison also ignores case ("IT" = "it").
* Update with a new ID checks that the new ID is free first, so a failed update never loses a student.
* Save file: one student per line, pre-order, fields separated by `|`. Loading rebuilds the same tree shape; bad lines are reported and skipped.

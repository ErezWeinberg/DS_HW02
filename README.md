# DS_HW02 — Racenion (Data Structures 1, Wet 2)

This repository contains a full implementation of the **Racenion** system for the Technion course **Data Structures 1 (234218), Spring 2026**.

The project models teams and contestants and supports dynamic operations such as:
- creating/removing teams,
- adding contestants,
- running duels,
- querying missions/experience/ordering by motivation,
- recruiting (merging) teams.

---

## Main Goals

The implementation is designed to support efficient online updates and queries by combining:
- **Union-Find (Disjoint Set Union)** for contestant/team-group relations,
- **AVL trees** for ordered team indexing,
- **hash table** for fast contestant lookup.

It also manages matrix-based skill arithmetic using modular operations provided by `Skill`.

---

## Public API (Racenion)

The required API is declared in:
- `/home/runner/work/DS_HW02/DS_HW02/Racenion26b2.h`

And implemented in:
- `/home/runner/work/DS_HW02/DS_HW02/Racenion26b2.cpp`

Supported operations:
- `add_team(int teamId)`
- `remove_team(int teamId)`
- `add_contestant(int contestantId, int teamId, const Skill& skill, int motivation, int missionsHad)`
- `duel(int teamId1, int teamId2)`
- `get_contestant_missions_number(int contestantId)`
- `get_team_experience(int teamId)`
- `get_ith_collective_motivation_team(int i)`
- `get_partial_team_skill(int contestantId)`
- `recruit(int recruitingTeamId, int recruitedTeamId)`

Each operation follows the assignment’s status contract via:
- `StatusType`
- `output_t<T>`

Both are defined in:
- `/home/runner/work/DS_HW02/DS_HW02/wet2util.h`

---

## Core Data Structures

Implemented in:
- `/home/runner/work/DS_HW02/DS_HW02/DataStructures.h`

### 1) Union-Find (`UFNode`)
- Represents contestants as nodes in disjoint sets (team components).
- Supports path-compression-based root finding.
- Stores accumulated transformations for:
  - partial skill composition,
  - mission offsets.
- Used to answer contestant-level queries efficiently after merges and duels.

### 2) Hash Table (`HashTable`)
- Open-addressing structure for mapping `contestantId -> UFNode*`.
- Handles dynamic growth using rehashing.
- Used for near-constant-time contestant existence and lookup operations.

### 3) AVL Tree (`AVLTree<Key, Value>`)
- Self-balancing binary search tree with:
  - insertion/removal/search,
  - order-statistics (`select(i)` using subtree sizes).
- Used in two indexes:
  - `teamsById` (key: `teamId`)
  - `teamsByMotivation` (key: `MotivationKey{motivation, teamId}`)

### 4) Team Aggregates (`Team`)
Each team stores:
- `teamExp` (experience points),
- `totalMotivation`,
- `totalSkill` (product of members’ skills),
- `uf_root` (representative in the union-find forest).

---

## Skill Representation

`Skill` is a 2x2 matrix under modular arithmetic (in `wet2util.h`):
- validity is determined by non-zero determinant modulo `SKILL_MOD`,
- multiplication composes skills,
- inverse supports relative skill transformations,
- determinant is used as effective scalar skill in comparisons.

---

## Runtime Driver and I/O

The command-driven executable entry point is:
- `/home/runner/work/DS_HW02/DS_HW02/main26b2.cpp`

It reads commands from standard input and prints formatted results.
This file is provided by the course and should be treated as fixed infrastructure.

---

## Tests

Test assets are located in:
- `/home/runner/work/DS_HW02/DS_HW02/tests/`

The helper script:
- `/home/runner/work/DS_HW02/DS_HW02/run_tests.py`

does the following:
1. Compiles all `.cpp` files with `g++ -std=c++14 -DNDEBUG -Wall`
2. Builds `main.exe`
3. Runs all `test*.in` files
4. Compares outputs against `test*.out`

Run all tests from repository root:

```bash
cd /home/runner/work/DS_HW02/DS_HW02
python3 run_tests.py
```

Run specific tests:

```bash
python3 run_tests.py -t 3 7 18
```

---

## Repository Layout

- `/home/runner/work/DS_HW02/DS_HW02/Racenion26b2.h` — required interface
- `/home/runner/work/DS_HW02/DS_HW02/Racenion26b2.cpp` — implementation
- `/home/runner/work/DS_HW02/DS_HW02/DataStructures.h` — custom DS implementations
- `/home/runner/work/DS_HW02/DS_HW02/wet2util.h` — shared utilities and `Skill`
- `/home/runner/work/DS_HW02/DS_HW02/main26b2.cpp` — command runner
- `/home/runner/work/DS_HW02/DS_HW02/tests/` — local tests
- `/home/runner/work/DS_HW02/DS_HW02/run_tests.py` — build/test script
- `/home/runner/work/DS_HW02/DS_HW02/readme.txt` — short test-running note

---

## Notes

- The assignment expects preserving public signatures in `Racenion26b2.h`.
- The implementation focuses on correctness with dynamic updates and efficient queries.
- For local validation, `run_tests.py` is the recommended workflow.

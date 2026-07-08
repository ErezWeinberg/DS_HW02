#ifndef DATA_STRUCTURES_H_
#define DATA_STRUCTURES_H_

#include "wet2util.h"

struct UFNode {
    int contestantId;
    UFNode* parent;
    int size;
    Skill skill_multiplier;
    int missions_offset;
    bool is_active;
    
    UFNode(int id, const Skill& s, int missions) : 
        contestantId(id), parent(this), size(1), 
        skill_multiplier(s), missions_offset(missions), is_active(true) {}
};

class HashTable {
private:
    struct HashEntry {
        int key;
        UFNode* val;
        bool used;
        HashEntry() : key(0), val(nullptr), used(false) {}
    };
    
    HashEntry* table;
    int capacity;
    int size;

    void rehash() {
        int new_capacity = capacity * 2;
        HashEntry* new_table = new HashEntry[new_capacity];
        
        for (int i = 0; i < capacity; ++i) {
            if (table[i].used) {
                int idx = table[i].key % new_capacity;
                while (new_table[idx].used) {
                    idx = (idx + 1) % new_capacity;
                }
                new_table[idx].key = table[i].key;
                new_table[idx].val = table[i].val;
                new_table[idx].used = true;
            }
        }
        
        delete[] table;
        table = new_table;
        capacity = new_capacity;
    }

public:
    HashTable(int initial_capacity = 16) : capacity(initial_capacity), size(0) {
        table = new HashEntry[capacity];
    }
    
    ~HashTable() {
        for (int i = 0; i < capacity; ++i) {
            if (table[i].used) {
                delete table[i].val;
            }
        }
        delete[] table;
    }

    void insert(int key, UFNode* val) {
        if (size * 2 >= capacity) {
            rehash();
        }
        int idx = key % capacity;
        while (table[idx].used) {
            if (table[idx].key == key) return;
            idx = (idx + 1) % capacity;
        }
        table[idx].key = key;
        table[idx].val = val;
        table[idx].used = true;
        size++;
    }

    UFNode* find(int key) {
        int idx = key % capacity;
        while (table[idx].used) {
            if (table[idx].key == key) {
                return table[idx].val;
            }
            idx = (idx + 1) % capacity;
        }
        return nullptr;
    }
};

struct Team {
    int teamId;
    int teamExp;
    int totalMotivation;
    Skill totalSkill;
    UFNode* uf_root;
    
    Team(int id) : teamId(id), teamExp(0), totalMotivation(0), totalSkill(Skill::identity()), uf_root(nullptr) {}
};

template <typename Key, typename Value>
class AVLTree {
private:
    struct Node {
        Key key;
        Value value;
        Node* left;
        Node* right;
        int height;
        int size;
        
        Node(Key k, Value v) : key(k), value(v), left(nullptr), right(nullptr), height(1), size(1) {}
    };

    Node* root;
    int num_elements;

    int getHeight(Node* node) {
        return node ? node->height : 0;
    }

    int getSize(Node* node) {
        return node ? node->size : 0;
    }

    int getBalance(Node* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }

    void update(Node* node) {
        if (node) {
            int hl = getHeight(node->left);
            int hr = getHeight(node->right);
            node->height = 1 + (hl > hr ? hl : hr);
            node->size = 1 + getSize(node->left) + getSize(node->right);
        }
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        update(y);
        update(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        update(x);
        update(y);
        return y;
    }

    Node* balance(Node* node) {
        update(node);
        int balanceFactor = getBalance(node);
        if (balanceFactor > 1) {
            if (getBalance(node->left) < 0) {
                node->left = rotateLeft(node->left);
            }
            return rotateRight(node);
        }
        if (balanceFactor < -1) {
            if (getBalance(node->right) > 0) {
                node->right = rotateRight(node->right);
            }
            return rotateLeft(node);
        }
        return node;
    }

    Node* insertNode(Node* node, Key key, Value value, bool& success) {
        if (!node) {
            success = true;
            return new Node(key, value);
        }
        if (key < node->key) {
            node->left = insertNode(node->left, key, value, success);
        } else if (key > node->key) {
            node->right = insertNode(node->right, key, value, success);
        } else {
            success = false;
            return node;
        }
        return balance(node);
    }

    Node* getMinValueNode(Node* node) {
        Node* current = node;
        while (current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    Node* removeNode(Node* node, Key key, bool& success) {
        if (!node) {
            success = false;
            return node;
        }
        if (key < node->key) {
            node->left = removeNode(node->left, key, success);
        } else if (key > node->key) {
            node->right = removeNode(node->right, key, success);
        } else {
            success = true;
            if (!node->left || !node->right) {
                Node* temp = node->left ? node->left : node->right;
                if (!temp) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }
                delete temp;
            } else {
                Node* temp = getMinValueNode(node->right);
                node->key = temp->key;
                node->value = temp->value;
                bool dummy;
                node->right = removeNode(node->right, temp->key, dummy);
            }
        }
        if (!node) return node;
        return balance(node);
    }

    Node* findNode(Node* node, Key key) {
        if (!node || node->key == key) return node;
        if (key < node->key) return findNode(node->left, key);
        return findNode(node->right, key);
    }

    Value selectNode(Node* node, int i) {
        int left_size = getSize(node->left);
        if (i == left_size + 1) return node->value;
        if (i <= left_size) return selectNode(node->left, i);
        return selectNode(node->right, i - left_size - 1);
    }

    void clear(Node* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    template <typename Func>
    void forEachNode(Node* node, Func f) {
        if (node) {
            forEachNode(node->left, f);
            f(node->value);
            forEachNode(node->right, f);
        }
    }

public:
    AVLTree() : root(nullptr), num_elements(0) {}

    ~AVLTree() {
        clear(root);
    }

    bool insert(Key key, Value value) {
        bool success = false;
        root = insertNode(root, key, value, success);
        if (success) num_elements++;
        return success;
    }

    bool remove(Key key) {
        bool success = false;
        root = removeNode(root, key, success);
        if (success) num_elements--;
        return success;
    }

    Value find(Key key) {
        Node* node = findNode(root, key);
        return node ? node->value : nullptr;
    }

    Value select(int i) {
        if (i < 1 || i > num_elements) return nullptr;
        return selectNode(root, i);
    }

    int size() const {
        return num_elements;
    }

    template <typename Func>
    void forEach(Func f) {
        forEachNode(root, f);
    }
};

struct MotivationKey {
    int motivation;
    int teamId;
    
    bool operator<(const MotivationKey& other) const {
        if (motivation != other.motivation)
            return motivation < other.motivation;
        return teamId < other.teamId;
    }
    bool operator>(const MotivationKey& other) const {
        return other < *this;
    }
    bool operator==(const MotivationKey& other) const {
        return motivation == other.motivation && teamId == other.teamId;
    }
};

inline UFNode* find_root(UFNode* x) {
    if (!x) return nullptr;
    if (x->parent != x) {
        UFNode* y = x->parent;
        UFNode* root = find_root(y);
        
        x->skill_multiplier = y->skill_multiplier * x->skill_multiplier;
        x->missions_offset += y->missions_offset;
        
        x->parent = root;
    }
    return x->parent;
}

inline UFNode* union_trees(UFNode* rootA, UFNode* rootB, Skill total_A) {
    if (!rootA) return rootB;
    if (!rootB) return rootA;
    
    if (rootA->size >= rootB->size) {
        rootB->parent = rootA;
        rootA->size += rootB->size;
        
        rootB->skill_multiplier = rootA->skill_multiplier.inv() * total_A * rootB->skill_multiplier;
        rootB->missions_offset = rootB->missions_offset - rootA->missions_offset;
        return rootA;
    } else {
        rootA->parent = rootB;
        rootB->size += rootA->size;
        
        Skill new_rootB_skill = total_A * rootB->skill_multiplier;
        Skill new_rootA_skill = new_rootB_skill.inv() * rootA->skill_multiplier;
        
        rootA->skill_multiplier = new_rootA_skill;
        rootB->skill_multiplier = new_rootB_skill;
        
        int new_rootB_missions = rootB->missions_offset;
        int new_rootA_missions = rootA->missions_offset - new_rootB_missions;
        
        rootA->missions_offset = new_rootA_missions;
        
        return rootB;
    }
}

#endif // DATA_STRUCTURES_H_

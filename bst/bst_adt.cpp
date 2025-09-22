#include <iostream>

using namespace std;

class Node {
public:
    int data;
    Node *lchild;
    Node *rchild;
};

class BST {
public:
    Node *root;

    BST() {
        root = nullptr;
    }

    void insert(int val) {
        /**
         * 1. Use 2 pointers, loop terminates when pointer 1 reaches null ie. reach the leaf
         * 2. check if val is > or < then curr node val, then move left or right
         */
        Node *t = new Node;
        t->data = val;
        t->lchild = nullptr;
        t->rchild = nullptr;
        if (!root) {
            root = t;
        } else {
            Node *p = root;
            Node *q = nullptr;
            while (p) {
                if (val < p->data) {
                    q = p;
                    p = p->lchild;
                } else if (val > p->data) {
                    q = p;
                    p = p->rchild;
                } else {
                    return;
                }
            }
            if (val > q->data) {
                q->rchild = t;
            } else {
                q->lchild = t;
            }
        }
    }

    Node *search(int key) {
        /**
         * While pointer is not null, go left or right
         */
        if (!root) { return nullptr; }
        Node *p = root;
        while (p) {
            if (p->data == key) {
                return p;
            } else if (p->data > key) {
                p = p->lchild;
            } else {
                p = p->rchild;
            }
        }
        return nullptr;
    }

    static void inorder_display(Node *p) {
        if (p) {
            inorder_display(p->lchild);
            cout << p->data << ", ";
            inorder_display(p->rchild);
        }
    }

    static Node *inorder_pre(Node *p) {
        /**
         * Returns inorder predecessor of a node
         */
        Node *curr = p;
        while (curr && curr->rchild) {
            curr = curr->rchild;
        }
        return curr;
    }

    static Node *inorder_succ(Node *p) {
        /*
         * Returns inorder successor of a node
         */
        Node *curr = p;
        while (curr && curr->lchild) {
            curr = curr->lchild;
        }
        return curr;
    }

    int height(Node *p) {
        if (p == nullptr) {
            return 0;
        }
        int x = height(p->lchild);
        int y = height(p->rchild);
        if (x > y) {
            return 1 + x;
        } else {
            return 1 + y;
        }
    }

    Node *del(Node *p, int val) {
        /**
         * Find the node
         * If it is a leaf node, just delete it
         * If it has one child, that child takes its place
         * If it has 2 child, fill its place with inorder predecessor (left, right right ...).
         * But if inorder predecessor is not a leaf, there needs to be more modifications
         * Steps:
         * 1. If current pointer is root, delete it
         * 2. If current pointer is null (leaf node), delete it
         * 3. while there is a pointer, if val is smaller than curr value, delete left else delete right
         * 4. Recursively search down the branch and connect the result of delete to the branch
         * 5. If we find the node to delete, get the height of its 2 sub trees
         * 6. If left is taller, get its inorder predecessor, and replace it with the in order predecessor. Else get the inorder successor
         * * the inorder successor/predecessor functions work for all cases
         * If there is 1 child, it will return that child since we look both left and right. If 2 child, it just returns the one from the taller tree
         * Once we replace curr node with the successor node, remember to delete the successor/predecessor node
         */
        if (p == nullptr) {
            return nullptr;
        }
        if (!p->lchild and !p->rchild) {
            if (p == root) {
                root = nullptr;
            }
            delete p;
            return nullptr; // delete leaf nodes
        }
        if (val < p->data) {
            p->lchild = del(p->lchild, val);
        } else if (val > p->data) {
            p->rchild = del(p->rchild, val);
        } else {
            if (height(p->lchild) > height(p->rchild)) {
                // inorder pre
                Node *inpre = inorder_pre(p->lchild);
                p->data = inpre->data;
                p->lchild = del(p->lchild, inpre->data);
            } else {
                // inorder succ
                Node *insuc = inorder_succ(p->rchild);
                p->data = insuc->data;
                p->rchild = del(p->rchild, insuc->data);
            }
        }
        return p;
    }
};

int main() {
    cout << "BST ADT Test Suite\n";
    cout << "==================\n";

    BST bst;

    // Test 1: Insert operations
    cout << "\n=== Insert Operations ===\n";
    bst.insert(50);
    cout << "Inserted 50: ";
    BST::inorder_display(bst.root);
    cout << "\n";

    bst.insert(30);
    bst.insert(70);
    bst.insert(20);
    bst.insert(40);
    bst.insert(60);
    bst.insert(80);
    cout << "Full tree: ";
    BST::inorder_display(bst.root);
    cout << "\n";

    // Test duplicate insertion
    bst.insert(50);
    cout << "After duplicate 50: ";
    BST::inorder_display(bst.root);
    cout << "\n";

    // Test 2: Search operations
    cout << "\n=== Search Operations ===\n";
    cout << "Search 40: " << (bst.search(40) ? "Found" : "Not Found") << "\n";
    cout << "Search 25: " << (bst.search(25) ? "Found" : "Not Found") << "\n";
    cout << "Search 80: " << (bst.search(80) ? "Found" : "Not Found") << "\n";

    // Test 3: Height calculation
    cout << "\n=== Height Calculation ===\n";
    cout << "Tree height: " << bst.height(bst.root) << "\n";

    // Test 4: Delete operations
    cout << "\n=== Delete Operations ===\n";

    // Delete leaf node
    cout << "Before deleting 20: ";
    BST::inorder_display(bst.root);
    cout << "\n";
    bst.root = bst.del(bst.root, 20);
    cout << "After deleting 20: ";
    BST::inorder_display(bst.root);
    cout << "\n";

    // Delete node with two children (root)
    cout << "Before deleting 50: ";
    BST::inorder_display(bst.root);
    cout << "\n";
    bst.root = bst.del(bst.root, 50);
    cout << "After deleting 50: ";
    BST::inorder_display(bst.root);
    cout << "\n";

    // Test 5: Edge cases
    cout << "\n=== Edge Cases ===\n";
    BST empty;
    cout << "Empty tree height: " << empty.height(empty.root) << "\n";
    cout << "Search in empty tree: " << (empty.search(10) ? "Found" : "Not Found") << "\n";

    cout << "\nAll tests completed!\n";
    return 0;
}

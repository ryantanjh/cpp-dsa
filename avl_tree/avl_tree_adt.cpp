#include <iostream>
#include <string>

using namespace std;

struct Node {
    Node *lchild;
    Node *rchild;
    int data;
};

class AVLTree {
public:
    Node *root;

    AVLTree() {
        root = nullptr;
    }

    int node_height(Node *p) {
        if (p == nullptr) {
            return 0;
        }
        int lh = node_height(p->lchild);
        int rh = node_height(p->rchild);
        if (lh > rh) {
            return lh + 1;
        } else {
            return rh + 1;
        }
    }

    int balance_factor(Node *p) {
        /**
         * Balance factor = height of left sub tree - right sub tree
         */
        return node_height(p->lchild) - node_height(p->rchild);
    }

    Node *ll_rotation(Node *p) {
        Node *pl = p->lchild;

        // move original node 2's r child to node 1's left child
        p->lchild = pl->rchild;

        // move node 2 up to node 1 position
        pl->rchild = p;
        if (root == p) {
            root = pl;
        }
        return pl;
    }

    Node *lr_rotation(Node *p) {
        Node *plr = p->lchild->rchild;
        Node *pl = p->lchild;

        // move plr's left and right children
        pl->rchild = plr->lchild;
        p->lchild = plr->rchild;

        // shift plr up to p
        plr->lchild = pl;
        plr->rchild = p;

        if (root == p) {
            root = plr;
        }
        return plr;
    }

    Node *rr_rotation(Node *p) {
        Node *pr = p->rchild;

        // move original node 2's left child to node 1's right child
        p->rchild = pr->lchild;

        // move node 2 to node 1's place
        pr->lchild = p;

        if (root == p) {
            root = pr;
        }
        return pr;
    }

    Node *rl_rotation(Node *p) {
        Node *pr = p->rchild;
        Node *prl = pr->lchild;

        // move prl left and right child
        p->rchild = prl->lchild;
        pr->lchild = prl->rchild;

        // move prl up
        prl->lchild = p;
        prl->rchild = pr;

        if (root == p) {
            root = prl;
        }
        return prl;
    }

    Node *insert(Node *p, int key) {
        if (!p) {
            Node *t = new Node;
            t->data = key;
            t->lchild = nullptr;
            t->rchild = nullptr;
            return t;
        }
        if (key < p->data) {
            p->lchild = insert(p->lchild, key);
        } else if (key > p->data) {
            p->rchild = insert(p->rchild, key);
        }

        // Make rotations
        if (balance_factor(p) > 1 && balance_factor(p->lchild) == 1) {
            // LL
            return ll_rotation(p);
        } else if (balance_factor(p) > 1 && balance_factor(p->lchild) == -1) {
            // LR
            return lr_rotation(p);
        } else if (balance_factor(p) < -1 && balance_factor(p->rchild) == 1) {
            // RL
            return rl_rotation(p);
        } else if (balance_factor(p) < -1 && balance_factor(p->rchild) == -1) {
            // RR
            return rr_rotation(p);
        }

        return p;
    }

    Node *in_pre(Node *p) {
        while (p && p->rchild) {
            p = p->rchild;
        }
        return p;
    }

    Node *in_succ(Node *p) {
        while (p && p->lchild) {
            p = p->lchild;
        }
        return p;
    }

    Node *del(Node *p, int key) {
        // base case
        if (!p) {
            return nullptr;
        }
        if (!p->lchild && !p->rchild) {
            if (p == root) {
                root = nullptr;
            }
            delete p;
            return nullptr;
        }

        // traversal
        if (key < p->data) {
            p->lchild = del(p->lchild, key);
        } else if (key > p->data) {
            p->rchild = del(p->rchild, key);
        } else {
            // delete a node with 2 children
            if (node_height(p->lchild) > node_height(p->rchild)) {
                Node *t = in_pre(p->lchild);
                p->data = t->data;
                p->lchild = del(p->lchild, t->data);
            } else {
                Node *t = in_succ(p->rchild);
                p->data = t->data;
                p->rchild = del(p->rchild, t->data);
            }
        }

        // make balance and rotations
        if (balance_factor(p) == 2 && balance_factor(p->lchild) == 1) {  // L1 Rotation
            return ll_rotation(p);
        } else if (balance_factor(p) == 2 && balance_factor(p->lchild) == -1) {  // L-1 Rotation
            return lr_rotation(p);
        } else if (balance_factor(p) == -2 && balance_factor(p->rchild) == -1) {  // R-1 Rotation
            return rr_rotation(p);
        } else if (balance_factor(p) == -2 && balance_factor(p->rchild) == 1) {  // R1 Rotation
            return rl_rotation(p);
        } else if (balance_factor(p) == 2 && balance_factor(p->lchild) == 0) {  // L0 Rotation
            return ll_rotation(p);
        } else if (balance_factor(p) == -2 && balance_factor(p->rchild) == 0) {  // R0 Rotation
            return rr_rotation(p);
        }
        return p;
    }

    void in_order_display(Node *p) {
        if (p) {
            in_order_display(p->lchild);
            cout << p->data << ", ";
            in_order_display(p->rchild);
        }
    }

    void print_tree(Node *root, string indent = "", bool last = true) {
        if (root != nullptr) {
            cout << indent;
            if (last) {
                cout << "└── ";
                indent += "    ";
            } else {
                cout << "├── ";
                indent += "│   ";
            }
            cout << root->data << " (BF: " << balance_factor(root) << ")" << endl;

            bool hasLeft = root->lchild != nullptr;
            bool hasRight = root->rchild != nullptr;

            if (hasLeft || hasRight) {
                if (hasRight) {
                    print_tree(root->rchild, indent, !hasLeft);
                }
                if (hasLeft) {
                    print_tree(root->lchild, indent, true);
                }
            }
        }
    }

};

int main() {
    AVLTree *tree = new AVLTree();

    cout << "=== AVL Tree Operations Demo ===" << endl << endl;

    // Test insertions
    cout << "1. Inserting values: 10, 20, 30, 40, 50, 25" << endl;
    tree->root = tree->insert(tree->root, 10);
    cout << "After inserting 10:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->insert(tree->root, 20);
    cout << "After inserting 20:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->insert(tree->root, 30);
    cout << "After inserting 30 (triggers RR rotation):" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->insert(tree->root, 40);
    cout << "After inserting 40:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->insert(tree->root, 50);
    cout << "After inserting 50 (triggers RR rotation):" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->insert(tree->root, 25);
    cout << "After inserting 25:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    // Test balance factors
    cout << "2. Testing balance factors:" << endl;
    cout << "Root balance factor: " << tree->balance_factor(tree->root) << endl;
    if (tree->root->lchild) {
        cout << "Left child balance factor: " << tree->balance_factor(tree->root->lchild) << endl;
    }
    if (tree->root->rchild) {
        cout << "Right child balance factor: " << tree->balance_factor(tree->root->rchild) << endl;
    }
    cout << endl;

    // Test more insertions to trigger different rotations
    cout << "3. Inserting more values to test different rotations:" << endl;
    tree->root = tree->insert(tree->root, 5);
    cout << "After inserting 5:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->insert(tree->root, 15);
    cout << "After inserting 15:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->insert(tree->root, 12);
    cout << "After inserting 12 (may trigger LR rotation):" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    // Test deletions
    cout << "4. Testing deletions:" << endl;
    cout << "Current tree:" << endl;
    tree->print_tree(tree->root);
    cout << endl;

    tree->root = tree->del(tree->root, 12);
    cout << "After deleting 12:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->del(tree->root, 25);
    cout << "After deleting 25:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    tree->root = tree->del(tree->root, 20);
    cout << "After deleting 20:" << endl;
    tree->print_tree(tree->root);
    cout << "Height: " << tree->node_height(tree->root) << endl << endl;

    // Final tree state
    cout << "5. Final tree state:" << endl;
    tree->print_tree(tree->root);
    cout << "In-order traversal: ";
    tree->in_order_display(tree->root);
    cout << endl;
    cout << "Tree height: " << tree->node_height(tree->root) << endl;
    cout << "Root balance factor: " << tree->balance_factor(tree->root) << endl;

    delete tree;
    return 0;
}
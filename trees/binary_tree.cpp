#include <iostream>
#include <queue>

using namespace std;

struct Node {
    Node *left_child;
    int data;
    Node *right_child;
};

Node *create_tree() {
    /**
     * Returns pointer to root node. Only stops if no more child nodes
     */
    Node *root_node = new Node;
    queue<Node *> q;
    int x;
    cout << "Enter Root value: " << endl;
    cin >> x;
    root_node->data = x;
    root_node->left_child = nullptr;
    root_node->right_child = nullptr;
    Node *curr = root_node;
    q.push(curr);

    while (!q.empty()) {
        curr = q.front();
        q.pop();
        // Create left child
        Node *new_left_child = new Node;
        int left_val;
        cout << "Enter Left child value: " << endl;
        cin >> left_val;
        if (left_val != -1) {
            new_left_child->data = left_val;
            new_left_child->left_child = nullptr;
            new_left_child->right_child = nullptr;
            curr->left_child = new_left_child;
            q.push(new_left_child);
        }
        // Create right child
        Node *new_right_child = new Node;
        int right_val;
        cout << "Enter right child value: " << endl;
        cin >> right_val;
        if (right_val != -1) {
            new_right_child->data = right_val;
            new_right_child->left_child = nullptr;
            new_right_child->right_child = nullptr;
            curr->right_child = new_right_child;
            q.push(new_right_child);
        }
    }
    return root_node;
}

void pre_order(Node *root) {
    Node *curr = root;
    if (curr) {
        cout << curr->data << ", ";
        pre_order(curr->left_child);
        pre_order(curr->right_child);
    }
}

void post_order(Node *root) {
    Node *curr = root;
    if (curr) {
        post_order(curr->left_child);
        post_order(curr->right_child);
        cout << curr->data << ", ";
    }
}

void in_order(Node *root) {
    Node *curr = root;
    if (curr) {
        in_order(curr->left_child);
        cout << curr->data << ", ";
        in_order(curr->right_child);
    }
}

void level_order(Node *root) {
    /**
     * 1. Insert root into queue
     * 2. While queue is not empty, pop from queue, and print/push the elements on its left and right
     */
    Node *curr = root;
    cout << root->data << ", ";
    queue<Node *> q;
    q.push(root);
    while (!q.empty()) {
        curr = q.front();
        q.pop();
        if (curr->left_child) {
            cout << curr->left_child->data << ", ";
            q.push(curr->left_child);
        }
        if (curr->right_child) {
            cout << curr->right_child->data << ", ";
            q.push(curr->right_child);
        }
    }
}

int count(Node *p) {
    /**
     * Travese the tree using any traversal method and add 1 on each call
     */
    if (p) {
        int x = count(p->left_child);
        int y = count(p->right_child);
        return x + y + 1;
    } else {
        return 0;
    }
}

int height(Node *p) {
    /**
     * Get the height of your left and right subtree. Take the taller tree
     * Getting height of sub tree is recursive
     */
    if (p) {
        int x = height(p->left_child);
        int y = height(p->right_child);
        if (x > y) {
            return x + 1;
        } else {
            return y + 1;
        }
    } else {
        return 0;
    }
}

int main() {
    Node *root = create_tree();
    pre_order(root);
    cout << endl;
    post_order(root);
    cout << endl;
    in_order(root);
    cout << endl;
    level_order(root);
    cout << endl;
    cout << "count: " << count(root) << endl;
    cout << "height: " << height(root) << endl;
    return -1;
}
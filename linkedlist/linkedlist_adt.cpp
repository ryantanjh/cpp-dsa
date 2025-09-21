#include <iostream>

using namespace std;

class Node {
public:
    int data;
    Node *next;
};

class LinkedList {

public:
    Node *first;

    LinkedList() {
        first = nullptr;
    }

    LinkedList(int A[], int n) {
        first = new Node;
        first->data = A[0];
        first->next = nullptr;
        Node *last;
        last = first;
        for (int i = 1; i < n; i++) {
            Node *new_node = new Node;
            new_node->data = A[i];
            new_node->next = nullptr;
            // link new node to curr
            last->next = new_node;
            // curr = new node
            last = new_node;
        }
    }

    ~LinkedList() {
        Node *p = first;
        while (first) {
            first = first->next;
            delete p;
            p = first;
        }
    };

    void display() {
        Node *p = first;
        while (p) {
            cout << p->data;
            if (p->next) {
                cout << "->";
            }
            p = p->next;
        }
        cout << endl;
    }

    int length() {
        Node *p = first;
        int count = 0;
        while (p) {
            count++;
            p = p->next;
        }
        return count;
    }

    int max() {
        int max = INT32_MIN;
        Node *p = first;
        while (p) {
            if (p->data > max) {
                max = p->data;
            }
            p = p->next;
        }
        return max;
    }

    Node *search(int val) {
        /**
         * Returns first pointer to node that equals value
         */
        Node *p = first;
        while (p) {
            if (p->data == val) {
                return p;
            }
            p = p->next;
        }
        return nullptr;
    }

    void insert(int index, int val) {
        /**
         * 1. Reach the point at which you want to insert
         * 2. Create a new node
         * 3. Set the new node to point to current's next, and current's next to point to new node
         */
        Node *curr = first;
        if (index < 0 || index > length()) {
            return;
        }
        if (index == 0) {
            Node *new_node = new Node;
            new_node->data = val;
            new_node->next = curr;
            first = new_node;
        } else {
            for (int i = 0; i < index - 1; i++) {
                curr = curr->next;
            }
            Node *new_node = new Node;
            new_node->data = val;
            new_node->next = curr->next;
            curr->next = new_node;
        }
    }

    void sorted_insert(int val) {
        /**
         * Same as insert but must reach value first
         */
        Node *p = first;
        Node *q = NULL;
        while (p && p->data < val) {
            q = p;
            p = p->next;
        }
        Node *temp = new Node;
        temp->data = val;
        temp->next = q->next;
        q->next = temp;
    }

    int del(int idx) {
        /**
         * Index 1 is 0 index
         * 1. Reach the index you want to delete. There needs to be a pointer to the node prior to it
         * 2. Set prior node to the current's next to remove it
         * 3. Delete current from memory
         */
        int x = -1;
        if (idx < 1 || idx > length()) {
            return x;
        }
        if (idx == 1) {
            Node *t = first;
            first = first->next;
            x = t->data;
            delete t;
            return x;
        } else {
            Node *p = first;
            Node *q = NULL;

            for (int i = 1; i < idx; i++) {
                q = p;
                p = p->next;
            }
            q->next = p->next;
            x = p->data;
            delete p;
            return x;
        }
    }

    bool is_sorted() {
        Node *p = first;
        int curr = p->data;
        while (p) {
            if (p->data < curr) {
                return false;
            }
            curr = p->data;
            p = p->next;
        }
        return true;
    }

    void reverse() {
        /**
         * Using sliding pointer algo.
         * Can copy all values of each node into array, then iterate through array backwards and change values in
         * node. But we don't want to change node values as they can require large memory
         * 3 pointers: first pointer is to keep moving forward
         * Second pointer is used to reverse the next to the third pointer
         * When we reverse the next, first pointer is still there to move onwards
         */
        Node *p = first;
        Node *q = nullptr;
        Node *r = nullptr;
        while (p) {
            r = q;
            q = p;
            p = p->next;
            q->next = r;
        }
        first = q;
    }

    void concat(LinkedList *l2) {
        /**
         * Reach last node and set next to the new list
         */
        Node *p = first;
        while (p->next) {
            p = p->next; // p stops at last pointer
        }
        p->next = l2->first;
    }

    void merge(LinkedList *l2) {
        /**
         * Merge 2 sorted list
         * 1. Set the first node
         * 2. Keep 2 pointers on both lists. Add the smaller one to the merge list and increment the curr pointer till
         * one of them reach the end
         * 3. Add the remaining elements of the other list
         */
        Node *p = first; // pointer for l1
        Node *q = l2->first; // pointer for l2
        Node *curr; // used to buid the merged list
        Node *final;
        if (p->data < q->data) {
            final = p;
            curr = p;
            p = p->next;
        } else {
            final = q;
            curr = q;
            q = q->next;
        }

        while (p && q) {
            if (p->data < q->data) {
                curr->next = p; // Link p node
                curr = p;
                p = p->next; // shift p onwards
                curr->next = nullptr;
            } else {
                curr->next = q;
                curr = q;
                q = q->next;
                curr->next = nullptr;
            }
        }
        // clear remaining
        while (p) {
            curr->next = p;
            p = p->next;
        }
        while (q) {
            curr->next = q;
            q = q->next;
        }
        first = final;
    }

    bool is_loop() {
        /**
         * Returns true if linkedlist is a loop
         * To check if it is loop, think of 2 cars in a circular track, one at twice the speed.
         * They will eventually collide (terminating condition for loop)
         * If one of the cars reach null, means there is no circle
         */
        Node *p = first;
        Node *q = first;

        do {
            p = p->next;
            q = q->next;
            q = q ? q->next : q;
            if (p == q) {
                return true;
            }
        } while (p && q);
        // q is null ie. list is linaer
        return false;
    }
};

int main() {
    int A[] = {1, 2, 3};
    LinkedList *l = new LinkedList(A, 3);
    l->display();
    cout << "length: " << l->length() << endl;
    cout << "max: " << l->max() << endl;
    Node *s = l->search(2);
    cout << "linear search: " << s->data << endl;

    cout << "====== Insert ====== " << endl;
    l->insert(0, 0);
    l->display();
    l->insert(4, 5);
    l->display();
    l->insert(4, 4);
    l->display();
    l->insert(7, 99);
    l->display();
    cout << "====== Insert that should fail ====== " << endl;
    LinkedList *l2 = new LinkedList;
    l2->insert(1, 11);
    l2->display();
    cout << "====== Insert into sorted element ====== " << endl;
    l->sorted_insert(90);
    l->display();
    l->sorted_insert(70);
    l->display();

    cout << "====== Delete ====== " << endl;
    int x = l->del(7); // delete 70
    cout << x << endl;
    l->display();
    l->del(1); //delete 0
    l->display();

    cout << "====== Sorted ====== " << endl;
    cout << "Is sorted: " << l->is_sorted() << endl;
    l->insert(3, 99);
    cout << "Is sorted: " << l->is_sorted() << endl;

    cout << "====== Reverse ====== " << endl;
    l->del(4);
    l->reverse();
    l->display();

    cout << "====== Concat ====== " << endl;
    l->reverse();
    int B[] = {96, 97, 99};
    LinkedList *l4 = new LinkedList(B, 3);
    l->concat(l4);
    l->display();
    cout << "====== Merge ====== " << endl;
    int C[] = {0, 80, 88, 98, 101};
    LinkedList *l5 = new LinkedList(C, 5);
    l5->display();
    l->merge(l5);
    l->display();

    cout << "====== Loop ====== " << endl;
    cout << "Not loop: " << l->is_loop() << endl;
    LinkedList *looped_ll = new LinkedList(C, 5);
    Node *p = looped_ll->first;
    while (p->next) {
        p = p->next;
    } // p is now at last node
    p->next = looped_ll->first;
    cout << "Loop: " << looped_ll->is_loop() << endl;
}






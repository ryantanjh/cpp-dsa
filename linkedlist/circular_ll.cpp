#include <iostream>

using namespace std;

struct Node {
    int data;
    Node* next;
};

class CircularLinkedList {
public:
    Node* head;

    CircularLinkedList() {
        head = nullptr;
    }

    CircularLinkedList(int A[], int sz) {
        Node* first_node = new Node;
        first_node->data = A[0];
        first_node->next = first_node;
        head = first_node;

        Node * curr = head;
        for (int i = 1; i < sz; i++) {
            Node* new_node = new Node;
            new_node->data = A[i];
            new_node->next = nullptr;
            // link curr to next node
            curr->next = new_node;
            curr = new_node;
        }
        curr->next = head;
    }
    void display() {
        Node * curr = head;
        do {
            cout << curr->data;
            if (curr->next != head) {
                cout << "->";
            }
            curr = curr->next;
        } while (curr != head);
        cout << endl;
    }

    int length() {
        int c = 0;
        Node* curr = head;
        do {
            c++;
            curr = curr->next;
        } while (curr != head);
        return c;
    }

    void insert(int val, int idx) {
        if (idx < 0 || idx > this->length()) { // [1,2,3]
            return;
        }
        if (idx == 0) {
            Node* new_node = new Node;
            new_node->data = val;
            new_node->next = head;
            Node* curr = head;
            do {
                curr = curr->next;
            } while (curr->next != head); // curr at last pointer

            curr->next = new_node;
            head = new_node;
        } else {
            Node * curr = head;
            for (int i = 1; i < idx; i++) {
                curr = curr->next;
            }
            Node * new_node = new Node;
            new_node->data = val;
            new_node->next = curr->next;
            curr->next = new_node;
        }
    }

    int del(int idx) {
        /**
         * Delete starting with index 1, last elem index = length
         */
        int x = 0;
        if (idx <= 0 || idx > length()) {
            return -1;
        }
        if (idx == 1) {
            Node * curr = head;
            while (curr->next != head) { // [1,2,3]
                curr = curr->next; // stop at last pointer
            }
            // point last pointer to head's next
            curr->next = head->next;
            // store head data in output
            x = head->data;
            // delete head;
            Node * temp = head;
            curr->next = head->next;
            head = head->next;
            delete temp;
        } else {
            Node* curr = head;
            for (int i = 1; i < idx - 1; i++) {
                curr = curr->next;
            }
            Node* next = curr->next;
            if (next->next) {
                curr->next = next->next;
            } else {
                curr->next = nullptr;
            }
            x = next->data;
            delete next;
        }
        return x;
    }
};

int main() {
    int A[6] = {1, 2, 3, 4, 6, 10};
    CircularLinkedList* cll = new CircularLinkedList(A, 6);
    cll->display();
    cout << "length: " << cll->length() << endl;
    cout << "===== Insert ========" << endl;
    cll->insert(0, 0); // insert at first
    cll->display();
    cll->insert(88, 3); // insert at index
    cll->display();
    cll->insert(90, cll->length()); // insert at last
    cll->display();

    cout << "===== Delete ========" << endl;
    int x = cll->del(1);
    cout << x << endl;
    cll->display();
    int y = cll->del(3);
    cout << y << endl; //88
    cll->display();
    int z = cll->del(7);
    cout << z << endl; //90
    cll->display();
}
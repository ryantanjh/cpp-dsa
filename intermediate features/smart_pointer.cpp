//
// Created by ryan tan on 1/4/26.
//
/*
 * Smart pointers are wrappers that manage lifecycle of heap allocated memory .
 * this means we dont need to free memory our selves
 */

#include "iostream"
#include <memory>
using namespace std;

struct Node {
    int value;
    Node(int v): value(v) {cout << "Created node: " << v << "\n";}
    ~Node() {cout << "Node destroyed: " << value << endl;}
};

int main() {
    // Unique pointers: Cannot transfer ownership
    auto a = std::make_unique<Node>(1);
    cout << "Node a: " << a << " , with value: " << a->value << endl;
    auto b = std::move(a);
    cout << "A is now null: " << a << endl;
    cout << "B is now node: " << b << endl;

    // shared pointer: can share ownership
    shared_ptr<Node> s1;
    {
        auto s2 = make_shared<Node>(2); // created node on the heap
        cout << "ref count: "<< s2.use_count() << endl; // ref count = 1
        s1 = s2;
        // ref count is the number of shared pointers pointing to an objetc
        cout << "ref count: "<< s2.use_count() << endl; // ref count = 2
    }
    // out of scope
    cout << "ref count after s2 is out of scope: "<< s1.use_count() << endl;
    s1.reset();
    cout << "ref count after s1 is reset (destroyed): "<< s1.use_count() << endl;
}
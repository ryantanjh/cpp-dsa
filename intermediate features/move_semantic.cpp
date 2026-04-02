//
// Created by ryan tan on 2/4/26.
//
#include <iostream>
#include <vector>
using namespace std;

// use move semantics when we want to avoid deep copying heap data (slow). instead we just transfer pointer ownership
// move sematic is safer than manually assigning pointer: deals with potential mememory leak

int main() {
    vector<int> a = {1, 2, 3, 4, 5};
    vector<int> a_copy = a;
    cout << a.size() << endl;
    cout << a_copy.size() << endl;

    // move semantics
    vector<int> moved = move(a);
    cout << a.size() << endl; // a is now empty
    cout << moved.size() << endl;

    // why use move semantics
    {
        int* a = new int[5];
        int* b = a;
        a = nullptr;
    } // scope ends, b vanishes but heap is not freed => leak
    // risk of memory leak: this memory can NEVER be reclaimed and you eventually run out of heap memory

    {
        vector<int> a = {1, 2, 3};
        vector<int> b = move(a);
    } // scope ends, b destructor run

}
#include <iostream>

using namespace std;

void insert(int A[], int n) {
    /**
     * n is the element in the array we want to insert
     * While the current element is greater than its parent,
     * we swap the current element with its parent
     * time: O(log n)
     */
    int temp, i = n;
    temp = A[n]; // temp stores the element we want to insert in last child position
    while (i > 1 && temp > A[i/2]) {
        A[i] = A[i / 2]; // move up the tree to parent
        i = i / 2;
    }
    // i is now in the right place as temp < its parent
    // assign value to i
    A[i] = temp;
}

void del(int A[], int n) {
    /**
     * For heap, only can delete the root element ie. highest element
     * 1. take out root element and store in temp. Then last element takes its place
     * Init i and j where i is current index and j is left child
     * 2. while j < n - 1 (stops when left child is the last element of heap
     * which is number of elements - 1
     * 3. Compare left child with right child, if right child is bigger, j = j + 1
     * We want to look at the bigger child
     * 4. If larger child is bigger than current node, swap their values
     * then curr node becomes that child
     * Move curr node to that child. Move child pointer to its left child. Loop continues
     * Else break out from the loop.
     * 5. Store the root value (in temp) at the last index
     */
     int root = A[1];
     A[1] = A[n];
     int i = 1;
     int j = i * 2;
     while (j < n - 1) {
         if (A[j] < A[j + 1]) {
             j = j + 1;
         }
         if (A[j] > A[i]) {
             int temp = A[j];
             A[j] = A[i];
             A[i] = temp;
             i = j;
             j = 2 * i;
         } else {
             break;
         }
     }
     A[n] = root;
}

void heap_sort(int A[]) {
    /**
     * 1. Takes in array of elements starting from index 1
     * 2. "delete" n elements 1 by 1
     * 3. time: O(nlogn)
     */
     for (int i = 7; i > 1; i--) {
         del(A, i);
     }
}

void create_heap(int A[]) {
    /**
     * Takes in array with elements starting from index 1
     * Creates a heap starting from index 1
     * time: O(nlogn)
     */
     // start from 2 as insert will swap with parent i/2
    for (int i = 2; i <= 7; i++) {
        insert(A, i);
    }
}

int main() {
    // create heap
    // time for creating heap: nlogn
    int A[] = {0,10,20,30,25,5,40,35};
    create_heap(A);
    for (int i = 0; i <= 7; i++) {
        cout << A[i] << ", ";
    }
    cout << endl;
    heap_sort(A);
    for (int i = 0; i <= 7; i++) {
        cout << A[i] << ", ";
    }
    cout << endl;
    return -1;
}
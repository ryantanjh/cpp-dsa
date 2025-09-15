/**
 * Example code for array functionality
 */
#include <iostream>

using namespace std;

void modify_first_elem(int A[], int n) {
    // bracket means it is a pointer to an Array. Arrays can only use pass by address
    A[0] = n; // modifies original array's first element
}

int *get_array(int n) {
    // function that returns array on heap
    int *p;
    p = new int[n];
    return (p);
}

int main() {
    // Static initialising of arrays
    int A[10] = {1, 2, 3};
    A[3] = 99;

    // iterating through arrays
    for (int i: A) {
        cout << i << endl; // uninitialised values will be 0
    }
    cout << "========" << endl;
    for (int i = 0; i < 5; i++) {
        cout << A[i] << endl;
    }
    cout << "========" << endl;
    // Variable initialising. Will assign garbage values
    int n;
    cout << "Enter size: " << endl;
    cin >> n;
    int B[n];
    B[0] = 2;
    for (int i: B) { // Only first element is 2. Rest is garbage
        cout << "B arr: " << i << endl;
    }

    cout << "===== heap array example ======" << endl;
    int arr_size = 5;
    int *C = get_array(arr_size); // get pointer to array on heap
    cout << "C pointer: " << C << endl;
    modify_first_elem(C, 99);
    for (int i = 0; i < arr_size; i++) { // First elem in arr modified to 99
        // accessing members of array on heap. Compiler doesnt know size of array on heap, so need to specify
        // range in for loop
        cout << "C elem: " << C[i] << endl; // note that for heap, the unassigned elems are 0
    }
    return 0;
}

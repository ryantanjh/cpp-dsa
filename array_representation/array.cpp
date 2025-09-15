#include "iostream"
using namespace std;

int main() {
    // Basics
    int A[5]; // init with garbage values
    int B[5] = {1, 2, 3, 4, 5}; // init with values
    int C[5] = {1, 2, 3}; // last 2 assigned 0
    int D[] = {1, 2, 3, 4, 5, 6}; // auto create array of size 6

    // Static Array
    int E[5]; // static array created in stack. Size of array decided at compile time. Memory allocated at run time
    E[10] = 2;
    // note how this doesnt throw exception: Dangerous! better to use containers
    cout << "E10 out of bounds: " << E[10] << endl; // Assigning memory which might override some other variable

    // Dynamic array
    int n;
    cout << "Enter value: " << endl;
    cin >> n;
    int F[n];
    F[0] = 3;

    // Array on heap
    int *p;
    p = new int[5];
    p[0] = 55; // pointer acts as a name of an array
    delete [] p;

    /** Stack array cannot be resized. heap array also cannot modify size of original array. Why? When we
     * declare an array of size n, memory allocated to that array is contiguous. eg. int A[3] => [][][]
     * However, we can resize an array in heap by creating a new array of larger size and changing the pointer of the
     * original array to the new one. This is not possible for stack array as there is no pointer
     */
     int *p1 = new int[5];
     int *p2 = new int[10];
     for (int i=0; i<5; i++) {
         p2[i] = p1[i];
     }
     delete [] p1;
     p1 = p2; // shift pointer
     p2 = NULL;
     p1[6] = 9;
    for (int i=0; i<10; i++) {
        cout << "p1 elems: " << p1[i] << endl; // everything set to 0
    }

    // 2D arrays
    // creating in stack
    int m1[3][4] = {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}}; // 3 rows, 4 columns
    cout << "m1: " << m1[0][0] << endl; // should print 1;

    // array of pointers
    int* m2[3]; // this array is on stack
    m2[0] = new int[4]; // heap
    m2[1] = new int[4];// heap
    m2[2] = new int[4];// heap
    m2[1][1] = 1;
    cout << "m2: " << m2[1][1] << endl;  // prints 1

    // double pointer
    int **m3; // this pointer is on stack
    m3 = new int*[3]; // create an array of pointers in heap
    m3[0] = new int[4];// heap
    m3[1] = new int[4];// heap
    m3[2] = new int[4];// heap
    m3[1][1] = 1;
    cout << "m3: " << m3[1][1] << endl;  // prints 1


    return 0;
}
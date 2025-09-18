#include "iostream"

using namespace std;

class Array {
private:
    int *A; // pointer to array on heap. dynamically allocated
    int size; // max size
    int curr_length; // curr length
public:
    Array(int sz) {
        A = new int[sz];
        size = sz;
        curr_length = 0;
    }

    ~Array() {
        delete[]A;
    }

    void display() {
        if (curr_length == 0) {
            cout << "Array is empty" << endl;
            return;
        }
        cout << "[";
        for (int i = 0; i < curr_length; i++) {
            cout << A[i] << ", ";
        }
        cout << "]" << endl;
    }

    void append(int elem) {
        /**
         * O(1)
         */
        if (curr_length < size) {
            A[curr_length] = elem;
            curr_length++;
        }
    }

    void insert(int idx, int elem) {
        /**
         * O(N) worst case if insert at idx 0. O(1) best case if insert at last index
         */
        if (idx >= 0 && idx <= curr_length) {
            for (int i = curr_length; i > idx; i--) {
                A[i] = A[i - 1];
                // shift value on the left to curr space. Don't shift if i = index as we want to insert there
            }
            A[idx] = elem;
            curr_length++;
        }
    }

    int del(int idx) {
        /**
         * Returns deleted elem
         * O(N) worst cast. O(1) best case
         */
        if (idx >= 0 && idx < curr_length) { // if length is 5, last elem index is 4
            int output = A[idx];
            for (int i = idx; i < curr_length - 1; i++) {
                A[i] = A[i + 1];
                A[i + 1] = 0; // clean up the next space to 0
            }
            curr_length--;
            return output;
        }
        return 0;
    }

    int linear_search(int val) {
        /**
         * Return index of val
         * O(N)
         */
        for (int i = 0; i < curr_length; i++) {
            if (A[i] == val) {
                return i;
            }
        }
        return -1; // not found
    }

    int binary_search(int val) {
        /**
         * Assumption: Array is already sorted
         * O(log(n))
         */
        int low = 0;
        int high = curr_length - 1; // index of last elem
        while (low <= high) {
            int mid = (low + high) / 2;
            if (A[mid] == val) {
                return mid;
            } else if (val > A[mid]) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }
        return -1; // not found
    }

    int get(int idx) {
        if (idx >= 0 && idx < curr_length) {
            return A[idx];
        }
        return -1;
    }

    void set(int idx, int val) {
        if (idx >= 0 && idx < curr_length) {
            A[idx] = val;
        }
    }

    int max() {
        int max = A[0];
        for (int i = 0; i < curr_length; i++) {
            if (A[i] >= max) {
                max = A[i];
            }
        }
        return max;
    }

    int min() {
        int min = A[0];
        for (int i = 0; i < curr_length; i++) {
            if (A[i] <= min) {
                min = A[i];
            }
        }
        return min;
    }

    int sum() {
        int sum = 0;
        for (int i = 0; i < curr_length; i++) {
            sum += A[i];
        }
        return sum;
    }

    float avg() {
        return static_cast<float>(this->sum()) / static_cast<float>(curr_length);
    }

    void reverse() {
        int B[curr_length];
        for (int i = curr_length - 1, j = 0; i >= 0; i--, j++) {
            B[j] = A[i];
        }
        for (int i = 0; i < curr_length; i++) {
            A[i] = B[i];
        }
    }

    void left_shift() {
        for (int i = 1; i < curr_length; i++) {
            A[i - 1] = A[i];
            A[i] = 0;
        }
        curr_length--; // left most disappears.
    }

    bool is_sorted() {
        for (int i = 0; i < curr_length - 1; i++) {
            if (A[i] > A[i + 1]) {
                return false;
            }
        }
        return true;
    }

    Array *merge(Array *arr2) {
        /**
         * Returns pointer to new array, merging 2 different arrays
         * Assumption: both arrays are sorted
         */
        Array *arr3 = new Array(curr_length + arr2->curr_length);
        int i = 0; // pointer for arr 1
        int j = 0; // pointer for arr 2

        while (i < curr_length && j < arr2->curr_length) {
            if (A[i] < arr2->A[j]) {
                arr3->append(A[i]);
                i++;
            } else {
                arr3->append(arr2->A[j]);
                j++;
            }
        }
        for (int a = i; a < curr_length; a++) {
            arr3->append(A[a]);
        }
        for (int b = j; b < arr2->curr_length; b++) {
            arr3->append(arr2->A[b]);
        }

        return arr3;
    }

    Array *array_union(Array *arr2) {
        /**
         * Assumes both arrays are sorted
         * O(m+n)
         */
        Array *arr3 = new Array(curr_length + arr2->curr_length);
        int i, j;
        i = j = 0;
        while (i < curr_length && j < arr2->curr_length) {
            if (A[i] < arr2->A[j]) {
                arr3->append(A[i]);
                i++;
            } else if (A[i] > arr2->A[j]) {
                arr3->append(arr2->A[j]);
                j++;
            } else {
                // both equal
                arr3->append(A[i]);
                i++;
                j++;
            }
        }
        while (i < curr_length) {
            arr3->append(A[i++]);
        }
        while (j < arr2->curr_length) {
            arr3->append(arr2->A[j++]);
        }
        return arr3;
    }

    Array *array_intersect(Array *arr2) {
        Array* arr3 = new Array(curr_length + arr2->curr_length);
        int i, j;
        i = j = 0;
        while (i < curr_length && j < arr2->curr_length) {
            if (A[i] < arr2->A[j]) {
                i++;
            } else if (A[i] > arr2->A[j]) {
                j++;
            } else {
                arr3->append(A[i]);
                i++;
                j++;
            }
        }
        return arr3;
    }

};

int main() {
    cout << "====== append ======" << endl;
    Array arr = Array(10);
    arr.display();
    arr.append(1);
    arr.append(2);
    arr.display(); // prints [1,2]
    cout << "====== insert ======" << endl;
    arr.insert(2, 3);
    arr.display(); // prints [1,2,3]
    arr.insert(0, 0);
    arr.display(); // prints [0,1,2,3]
    cout << "===== delete =======" << endl;
    int x = arr.del(2);
    arr.display(); // prints [0,1,3]
    cout << x << endl; // prints 2
    cout << "======= delete =====" << endl;
    int y = arr.del(0);
    arr.display(); //prints [1,3]
    cout << "====== Delete outside index ======" << endl;
    int z = arr.del(100);
    arr.display(); // prints [1,3]
    cout << z << endl;
    cout << "====== Append 7 ======" << endl;
    arr.append(7); // prints. [1,3,7]
    arr.display();
    cout << "====== Linear Search ======" << endl;
    cout << arr.linear_search(7) << endl; // index 2
    cout << "====== Binary Search ======" << endl;
    cout << arr.binary_search(3) << endl; // index 1
    cout << "====== Avg ======" << endl;
    arr.display();
    cout << arr.avg() << endl;
    cout << "====== Reverse ======" << endl;
    arr.reverse();
    arr.display(); // [7,3,1]
    cout << "====== Left shift ======" << endl;
    arr.insert(0, 9);
    arr.insert(0, 11);
    arr.insert(0, 13);
    arr.display();
    arr.left_shift();
    arr.display();
    cout << "====== is sorted ======" << endl;
    cout << arr.is_sorted() << endl;
    arr.reverse();
    cout << arr.is_sorted() << endl; //[1,3,7,9,11]
    cout << "====== Merge ======" << endl;
    Array *arr2 = new Array(3); // [5,9,16]
    arr2->append(5);
    arr2->append(9);
    arr2->append(16);
    arr2->display();
    Array *arr3 = arr.merge(arr2);
    arr3->display(); //[1,3,5,7,9,9,11,16]
    cout << "====== Union ======" << endl;
    Array *arr4 = arr2->array_union(arr3);
    arr4->display(); // [1,3,5,7,9,9,11,16]
    cout << "====== Intersect ======" << endl;
    Array *arr5 = arr2->array_intersect(arr3);
    arr5->display(); // [5,9,16]
}
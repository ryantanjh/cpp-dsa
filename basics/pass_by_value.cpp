/**
 * Pass by value copies the whole data structure in the function memory
 */

#include <iostream>
using namespace std;

struct Test {
	int A[5]; 
};

void pass_by_val(struct Test t) {
	t.A[0] = 70; // t1 is copied. Original is still unchanged
}


int main() {
	struct Test t = {{1,2,3,4,5}};
	pass_by_val(t); // pass by value t into function, modifies first elem of arr to be 70
	for (int i = 0; i < 5; i++) {
		cout << t.A[i] << endl; // notice how original t is still unchanged
	}
}

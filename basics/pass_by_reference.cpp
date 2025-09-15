/**
 * Pass by reference creates an alias to the original object and allows us to modify it
 */

#include <iostream>
using namespace std;

struct Test {
	int A[5]; 
};

void pass_by_reference(Test& x) {
	x.A[0] = 70; // gives new name and modifies original data
}

int main() {
	struct Test t = {{1,2,3,4,5}};
	pass_by_reference(t);
	for (int i = 0; i < 5; i++) {
		cout << t.A[i] << endl; // notice how original t is modified
	}
	return 0; 
}

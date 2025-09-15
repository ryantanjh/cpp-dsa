#include <iostream>
using namespace std; 

void fun(int A[], int n) {
	// bracket means it is a pointer to an Array. Arrays can only use pass by address

	A[0] = 9; // can modify original array 
	for (int i = 0; i < n; i++) {
		cout << A[i] << endl; 
	}
}

int * get_array(int n) {
	// function that returns array
	int *p; 
	p = new int[n]; 
	return (p); 
}

int main() {
	int A[5] = {1,2,3,4,5}; 
	fun(A, 5); 

	cout << "-------------" << endl; 
	int *B = get_array(3); 
	B[0] = 66; 
	B[1] = 10;
	B[2] = 11; 
	fun(B, 3); 
	return 0; 
}

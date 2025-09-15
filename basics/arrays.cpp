#include <iostream> 
using namespace std; 

int main() {
	// Static initialising of arrays
	int A[5] = {1,2,3}; 
	A[3] = 99; 

	// Initialising not all values: Remaining variables are assigned as 0

	// iterating through arrays
	for (int i : A) {
		cout << i << endl; 
	}

	for (int i=0; i < 5; i++) {
		cout << A[i] << endl; 
	}

	// Variable initialising. Will assign garbage values
	int n; 
	cout << "Enter size: " << endl; 
	cin >> n; 
	int B[n]; 
	B[0] = 2; 
	for (int i : B) {
		cout << "B arr: " << i << endl; 
	}
	return 0; 
}

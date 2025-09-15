#include <iostream> 
using namespace std; 


int add(int a, int b) {
	int c = a + b; 
	return (c);  
}

void swap(int x, int y) {
	int temp; 
	temp = x; 
	x = y; 
	y = temp; 
}

void swap2(int *x, int *y) {
	int temp; 
	temp = *x; 
	*x = *y; 
	*y = temp; 
}

void swap3(int &a, int &b) {
	int temp; 
	temp = a; 
	a = b; 
	b = temp; 
}

int main() {
	//pass by value. Values are copied into variables in function. Function variables are deleted when it ends
	int x = 2; 
	int y = 3; 
	int z = add(x, y); 
	cout << z << endl; 
	
	// pass by value for swap
	swap(x, y); 
	cout << "x is 2: " << x << ", y is 3: " << y << endl;  

	// call by address. Passes address to function. Needs de-referencing. Allows us to modify the original variables
	swap2(&x, &y);
	cout << "x is 3: " << x << ", y is 2: " << y << endl; 

	// call by reference. Dont use unless function is simple. Easy to introduce bugs since function scope is technically part of main
	swap3(x, y);
	cout << "x is 2: " << x << ", y is 3: " << y << endl; 
}

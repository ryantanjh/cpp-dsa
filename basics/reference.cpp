/*
A reference is just another name for a variable
*/

#include <iostream> 
using namespace std; 

int main() {
	int a = 10; 
	int  &r = a; 
	
	cout << a << endl; 
	r++; 
	cout << r << endl; 

	int b = 25; 
	r = b; 
	cout << r << endl; 
	cout << a << endl; 
}

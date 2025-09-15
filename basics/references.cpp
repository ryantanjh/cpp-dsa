/*
A reference is just another name for a variable
*/

#include <iostream> 
using namespace std; 

int main() {
	int a = 10; 
	int  &r = a; 

    /**
     * Notice how a and r are essentially the same
     */
	cout << a << endl; 
	r++; 
	cout << r << endl;
    cout << a << endl;

    cout << "===========" << endl;
    /**
     * a is still the same as r, as we assign new value to r
     */
	int b = 25; 
	r = b; 
	cout << r << endl; 
	cout << a << endl; 
}

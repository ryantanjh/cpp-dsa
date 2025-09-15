/**
- pointers are used to store the address of a variable
1.  access things external to program, which runs in main memory and stack. 
	- eg. Heap memory, disk memory, keyboard, external network
2.  used to pass parameters
**/
#include <stdlib.h>
#include <iostream>
using namespace std; 

int main() {
	int a = 10; 
	int *p; 
	p = &a; 
	cout << "address of p1: " << p << endl; // address
	cout << "value of a: " << a << endl; // variable
	cout << "dereference p1: " << *p << endl; // de-referencing

	// accessing heap memory(C language)
	int *p2; 
	p2 = (int *)malloc(5*sizeof(int)); // assigns 10 bytes of memory in the heap, and pointer will point to address of the first memory address ->[][][][][]
	cout << "pointer2: " << p2 << endl; 
	// malloc returns void pointer, so need to typecast

	// accessing heap memory in c++
	int *p3; 
	p3 = new int[5]; 
	cout << "pointer3: " << p3 << endl; 

	p3[0] = 66; 
	cout << "Printing elements in array" << endl ; 
	for (int i=0; i < 5; i++) {
		cout << p3[i] << endl; 
	}
	// release memory
	delete [] p3; // for array need to add the [] 

	// size of pointers
	cout << "size of p1: " << sizeof(p) << endl; 
	cout << "size of p2: " << sizeof(p2) << endl; 
	cout << "size of p3: " << sizeof(p3) << endl; 
	
	cout << "--- Pointer to structure -- " << endl; 

	struct Rectangle {
		int l; 
		int b; 
	};
	struct Rectangle r = {10,5}; // r is 8 bytes (4 bytes each)
	struct Rectangle *pr = &r; // pointer takes 8 bytes in modern systems
	cout << "size of r: " << sizeof(r) << endl; 
	cout << "size of pr: " << sizeof(pr) << endl; 

	// accessing variables for pointe
	(*pr).l = 17; 
	pr->b = 19; 
	cout << "new length: " << r.l << endl; ; 
	cout << "new breadth: " << r.b << endl; ; 
	
	// creating struct in heap using pointer 
	struct Rectangle *ph = new Rectangle{1,2}; 
	cout << "ph length: " << ph->l << endl; 
	cout << "ph breadth: " << ph->b << endl; 
	delete ph; 
}	

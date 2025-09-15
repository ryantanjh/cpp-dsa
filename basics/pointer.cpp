/**
Code shows examples of basic pointer functionality
Pointers are used to store the address of a variable
1.  access things external to program, which runs in main memory and stack. 
	- eg. Heap memory, disk memory, keyboard, external network
    - Think of pointers as gateway to external resources. We need to de-allocate when done using
2.  used to pass parameters to functions
**/
#include <stdlib.h>
#include <iostream>
using namespace std; 

int main() {
	int a = 10; 
	int *p; 
	p = &a; 
	cout << "address of p1: " << p << endl; // pointer shows address
	cout << "dereference p1: " << *p << endl; // de-referencing a pointer shows its value

    cout << "===========" << endl;
	// accessing heap memory(C language)
	int *p2;
    // assigns 10 bytes of memory in the heap, and pointer will point to address of the first memory address ->[][][][][]
    p2 = (int *)malloc(5*sizeof(int));
	cout << "pointer2: " << p2 << endl; 
	// malloc returns void pointer, so need to typecast

	// accessing heap memory in c++
	int *p3; 
	p3 = new int[5]; 
	cout << "pointer3: " << p3 << endl;

    cout << "===========" << endl;
	p3[0] = 66; 
	cout << "Printing elements in array: " << endl ;
	for (int i=0; i < 5; i++) {
		cout << p3[i] << endl; 
	}
	// release memory
	delete [] p3; // for array need to add the [] 
    cout << "===========" << endl;

	// size of pointers => depends on compiler
	cout << "size of p1: " << sizeof(p) << endl; 
	cout << "size of p2: " << sizeof(p2) << endl; 
	cout << "size of p3: " << sizeof(p3) << endl;

    cout << "===========" << endl;
	cout << "--- Pointer to class/structure -- " << endl;
	struct Rectangle {
		int l; 
		int b; 
	};
	struct Rectangle r = {10,5}; // r is 8 bytes (4 bytes each)
	struct Rectangle *pr = &r; // pointer takes 8 bytes in modern systems
	cout << "size of r: " << sizeof(r) << endl; 
	cout << "size of pr: " << sizeof(pr) << endl; 

	// 2 different methods to access class variables via pointer
	(*pr).l = 17; 
	pr->b = 19;
	cout << "new length: " << r.l << endl;
	cout << "new breadth: " << r.b << endl;

    cout << "===========" << endl;
	// creating struct in heap using pointer 
	struct Rectangle *ph = new Rectangle;
    ph->b = 1;
    ph->l = 9;
	cout << "ph length: " << ph->l << endl; 
	cout << "ph breadth: " << ph->b << endl; 
	delete ph; 
}	

#include <iostream> 
using namespace std; 

struct Rectangle {
	int l; 
	int b; 
};

struct Test {
	int A[5]; 
	int n; 
};

void fun(struct Test t) {
	t.A[0] = 70; // t1 is copied. Original is still unchanged
}

struct Rectangle *heapRectangle() {
	// creates rectangle on heap and returns its pointer
	struct Rectangle *p = new Rectangle; 
	p->l = 5; 
	p->b = 3; 
	return p; 
}

int area(struct Rectangle r) {
	// pass strcuture by value. Copies the whole structure into new memory
	return (r.l * r.b); 
}

void changeLength(struct Rectangle *p) {
	// pass by address modifies original struct directly
	p->l = 100; 
}

int main() {
	struct Rectangle r1 = {10, 5}; 
	changeLength(&r1); 
	int a = area(r1); 
	cout << "area : " << a << endl; 

	cout << "--------" << endl; 
	struct Test t = {{1,2,3,4,5}, 5}; 
	fun(t); 
	for (int i = 0; i < 5; i++) {
		cout << t.A[i] << endl; 
	}

	cout << "=========" << endl;
	struct Rectangle *ptr = heapRectangle();
	cout << ptr->l << ", " << ptr->b << endl;
	delete ptr; // Free the allocated memory
	return 0; 
}

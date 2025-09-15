/**
1. defining structures
2. initialising structures
3. accessing members
4. Array of structs 
**/
#include <iostream>

struct Rectangle {
	int length; 
	int breadth; 
}; 

int main() {
	struct Rectangle r; 
	r.length = 2; 
	r.breadth = 5; 
	std::cout << r.length << std::endl; 
	
	struct Rectangle r2 = {2,3}; 
	std::cout << r2.breadth << std::endl; 

	struct Rectangle r3; 
	std::cout << r3.length << std::endl; 

	struct Rectangle rectangleArr[3] = {{2,3}, {4,5}}; 
	std::cout << rectangleArr[0].length << std::endl;
	
	std::cout << "err: " <<  rectangleArr[2].length << std::endl; 

	struct Rectangle r4 = {5,6}; 
	rectangleArr[2] = r4; 
	std::cout << "rectangle exists: " << rectangleArr[2].length << std::endl; 
	return 1; 
}

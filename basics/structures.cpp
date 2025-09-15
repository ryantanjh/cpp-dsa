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
	std::cout << "Length of r: " << r.length << std::endl;
	
	struct Rectangle r2 = {2,3}; 
	std::cout << "Breadth of r2: " << r2.breadth << std::endl;

    /**
     * See how if we create an obj but do not initialise its members, it is garbabge values
     */
	struct Rectangle r3;
	std::cout << "Length of r3: " << r3.length << std::endl;
    std::cout << "Since uninitalised, we see garbage value for r3 length" << std::endl;

    /**
     * Notice how length of third rectangle is 0. this is because the way we initialise the array actually created
     * a third rectangle with default 0 values
     */
	struct Rectangle rectangleArr[3] = {{2,3}, {4,5}}; 
	std::cout << "Length of first rectangle: " << rectangleArr[0].length << std::endl;
	std::cout << "Printing third rectangle length without initialising: " <<  rectangleArr[2].length << std::endl;


	struct Rectangle r4 = {5,6}; 
	rectangleArr[2] = r4; 
	std::cout << "Printing third rectangle length after initialising: " << rectangleArr[2].length << std::endl;
	return 1; 
}

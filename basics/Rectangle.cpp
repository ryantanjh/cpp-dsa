/**
 * Simple rectangle class to show default constructor, getter, setter, destructor
 */

#include <iostream>
using namespace std; 

class Rectangle 
{
	private: 
		// note that fields cannot be same as function name. Or use this -> length to be safe
		int length; 
		int breadth; 

	public: 
		Rectangle() {
            cout << "Default Constructor is called for rectangle (" << this->getLength() << "," << this->getBreadth() << ")" << endl;
            length = breadth = 1; // default members
        }
		Rectangle(int length, int breadth)
		{
			this->length = length; 
			this->breadth = breadth;
            cout << "Constructor is called for rectangle (" << this->getLength() << "," << this->getBreadth() << ")" << endl;
		}
		int area()
		{
			return length * breadth; 
		}
		int getLength() {return length;}
		int getBreadth() {return breadth;}
		void setLength(int l)
		{
			length = l;
		}
		~Rectangle(){
            cout << "Destructor is called for rectangle (" << this->getLength() << "," << this->getBreadth() << ")" << endl;
        };
};


int main() {
	Rectangle r(10,5);
	cout<< "area: " << r.area() << endl; 
	cout << "length: " << r.getLength() << endl; 
	r.setLength(5); 
	cout << "new area: " << r.area() << endl;
    cout << "=======" << endl;

    Rectangle r2;
    cout << "default length: " << r2.getLength() << endl;
    /**
     * Notice how after we exit the if block, the destructor for r3 is called, meaning we cannot
     * access it outside this scope
     * Destructors for r1 and r2 are called when exiting main()
     */
    if (true) {
        Rectangle r3(9, 9);
    }
}

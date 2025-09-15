#include <iostream>
using namespace std; 

class Rectangle 
{
	private: 
		// note that fields cannot be same as function name. Or use this -> length to be safe
		int length; 
		int breadth; 

	public: 
		Rectangle() {length = breadth = 1;}
		Rectangle(int length, int breadth)
		{
			this->length = length; 
			this->breadth = breadth; 
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
		~Rectangle(){};  

};


int main() {
	Rectangle r(10,5); 
	cout<< "area: " << r.area() << endl; 
	cout << "length: " << r.getLength() << endl; 
	r.setLength(5); 
	cout << "new area: " << r.area() << endl; 
}

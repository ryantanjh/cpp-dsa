/**
 * Demonstrate how to create a class with generic type
 */

#include <iostream>
using namespace std; 

template<class T> 
class Arithametic
{
	private: 
		T a; 
		T b;
	public: 
		Arithametic(T a, T b); 
		T add(); 
};


template<class T> 
Arithametic<T>::Arithametic(T a, T b)
{
	this->a = a; // this is a pointer to the class object, so we use -> to access pointer member values
	this->b = b; 
};

template <class T> 
T Arithametic<T>::add()
{
	T c; 
	c = a + b; 
	return c; 
};

int main() 
{
	Arithametic<int> a(1,2); 
	cout << a.add() << endl; 

	Arithametic<float> a2(2.1,3.5); 
	cout << a2.add() << endl; 
}	

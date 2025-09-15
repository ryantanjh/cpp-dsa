/**
 * Code to show how to declare classes and use scope resolution to define implementations
 */
#include <iostream>
using namespace std;

// Class declaration
class Rectangle2 {
private:
    int l;
    int b;

public:
    Rectangle2();
    Rectangle2(int length, int breadth);
    int area();
    int getLength();
    int getBreadth();
    void setLength(int length);
    void setBreadth(int breadth);
    ~Rectangle2();
};

// Implementation using scope resolution
Rectangle2::Rectangle2() {
    l = b = 1;
}

Rectangle2::Rectangle2(int length, int breadth) {
    l = length;
    b = breadth;
}

int Rectangle2::area() {
    return l * b;
}

int Rectangle2::getLength() {
    return l;
}

int Rectangle2::getBreadth() {
    return b;
}

void Rectangle2::setLength(int length) {
    l = length;
}

void Rectangle2::setBreadth(int breadth) {
    b = breadth;
}

Rectangle2::~Rectangle2() {
    // Destructor implementation
}

int main() {
    Rectangle2 r(10, 5);
    cout << "area: " << r.area() << endl;
    cout << "length: " << r.getLength() << endl;
    r.setLength(5);
    cout << "new area: " << r.area() << endl; // Should show 25
    
    return 0;
}

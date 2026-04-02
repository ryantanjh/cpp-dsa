//
// Created by ryan tan on 2/4/26.
//
#include "iostream"
#include <string>
using namespace std;
// interfaces: no data, no implementaion
class IAnimal {
public:
    virtual void speak() const = 0;
    virtual void move() const = 0;
    virtual ~IAnimal()  = default;
};

// abstract class : partial implementation
class Animal: public IAnimal {
protected:
    string name;
    int age;
public:
    Animal(const string& n, int a): name(n), age(a) {}
    void describe() const {
        cout << "Animal : " << name << " of age: " << age << endl;
    }
    void speak() const override = 0;
    void move() const override = 0;
};

// ============================================================
// CONCRETE CLASSES
// ============================================================
class Dog : public Animal {
public:
    Dog(const std::string& n, int a) : Animal(n, a) {}

    void speak() const override {
        std::cout << name << " says: Woof!\n";
    }

    void move() const override {
        std::cout << name << " runs on four legs.\n";
    }
};

class Bird : public Animal {
public:
    Bird(const std::string& n, int a) : Animal(n, a) {}

    void speak() const override {
        std::cout << name << " says: Tweet!\n";
    }

    void move() const override {
        std::cout << name << " flies with wings.\n";
    }
};

class Fish : public Animal {
public:
    Fish(const std::string& n, int a) : Animal(n, a) {}

    void speak() const override {
        std::cout << name << " says: ...(silence)\n";
    }

    void move() const override {
        std::cout << name << " swims with fins.\n";
    }
};
int main() {
    Dog  dog("Dog", 3);
    Bird bird("Bird", 1);
    Fish fish("Fish", 2);

    Animal* animals[] = { &dog, &bird, &fish };

    for (Animal* a : animals) {
        a->describe();
        a->speak();
        a->move();
        std::cout << "---\n";
    }
}
// #include <iostream>

// class MyClass {
// private:
//     int value;

// public:
//     MyClass(int val) : value(val) {}

//     // Const member function
//     int getValue() const {
//         return value;
//     }

//     // Non-const member function
//     void setValue(int val) {
//         value = val;
//     }
// };

// int main() {
//     const MyClass obj(10); // const object

//     std::cout << "Value: " << obj.getValue() << std::endl; // Allowed

//     // obj.setValue(20); // Error: Cannot call non-const member function on const object

//     return 0;
// }


#include <iostream>

class MyClass {
private:
    int data;
    mutable int accessCount;

public:
    MyClass(int d) : data(d), accessCount(0) {}

    int getData() const {
        accessCount++; // Allowed because accessCount is mutable
        return data;
    }

    int getAccessCount() const {
        return accessCount;
    }
};

int main() {
    const MyClass obj(42);

    std::cout << "Data: " << obj.getData() << std::endl;
    std::cout << "Access Count: " << obj.getAccessCount() << std::endl;

    return 0;
}


//Declaring an object as const ensures its immutability.
//Only const member functions can be invoked on const objects.
//Use mutable for fields that need to be modified even in const objects.
//This feature is particularly useful in scenarios like read-only 
//configurations or objects representing constant data.
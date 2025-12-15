#include <iostream>
using namespace std;

//A void pointer is a pointer that has no associated data type with it. 
//A void pointer can hold an address of any type and can be typecasted to any type.

int main() {
    void * ptr;
    int i = 10;
    // assign int address to void
    ptr = & i;
    cout << "Address of variable i " << & i << endl;
    cout << "Address where the void pointer is pointing " << ptr <<  endl;
    return 0;
}
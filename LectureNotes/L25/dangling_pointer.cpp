//A dangling pointer is a pointer pointing to a memory location that 
//has been freed (or deleted). There are three different ways 
//where Pointer acts as a dangling pointer.


//a)Function Call
//
//The pointer pointing to the local variable becomes dangling when the local variable is not static.
//
//Example:

#include<iostream>
using namespace std;
int * fun() {
    int x = 10;
    return &x;
}
int main() {
    int * p = fun();
    // p points to something which is not 
    // valid anymore 
    cout << * p;
    return 0;
}

//b) Deallocation of memory
//
//Deallocating a memory pointed by a pointer causes a dangling pointer.
//
//Example:

#include<iostream>
using namespace std;
int main() {
    //dynamic memory allocation.
    int * p = (int * ) malloc(sizeof(int));
    //after calling free() p becomes a dangling pointer
    free(p);
    //now p no more a dangling pointer.
    p = NULL;
    return 0;
}
 

//c) Variable goes out of scope
//
//When a pointer goes out of scope where it is valid, then it becomes a dangling pointer.
//
//Example:

#include<iostream>
using namespace std;
void min() {
    int * ptr;
    //..... //any code statements
    {   int ch;
        ptr = & ch;
    }
    //....
    // Here ptr is dangling pointer
}
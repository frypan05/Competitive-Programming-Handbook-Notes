#include<bits/stdc++.h>
using namespace std;

//Uninitialized pointers are known as wild pointers because 
//they point to some arbitrary memory location 
//and may cause a program to crash or behave unexpectedly.

int main() {
    int * p; /* wild pointer */
    int a = 10;
    p = & a; /* p is not a wild pointer now*/
    * p = 12; /* This is fine. Value of a is changed */
    return 0;
}
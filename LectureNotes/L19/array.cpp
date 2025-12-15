#include<iostream>
#include<array>

using namespace std;

int main() {
    int basic[3]={1,2,3};

    array<int,4> a={1,2,3,4}; 
    //STL array is static and its implementation is by the basic array
    int size=a.size(); //Returns the size of the array.

    for(int i=0;i<size;i++){
        cout<<a[i]<<endl;
    }

    cout<<"element at 2nd index: "<<a.at(2)<<endl;
    cout<<"empty or not: "<<a.empty()<<endl;  //returns boolean value
    cout<<"first element "<<a.front()<<endl; 
    cout<<"last element "<<a.back()<<endl;

    a.fill(10);  // Fills the entire array with 10
    array<int,4> b=a;//Copying the Array

    return 0;
}

/*

Summary of Time Complexities for std::array:
Operation	Time Complexity
Access an element	O(1)
Modify an element	O(1)
Retrieve size	O(1)
Access first/last element	O(1)
Fill the array	O(n)
Iteration	O(n)
Copying the array	O(n)

*/
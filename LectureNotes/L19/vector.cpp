//vector is a dynamic array

//a vector doubles in size when it is fully filled in such a way
//that lets say a vector of size 4 is filled and when we try to add
//a 5th element, it doubles in size. This operation takes O(n) time
// a new vector is created with double the size and the contents of 
//previous vector are copied to it and the previous vector is then dumped.

#include<iostream>
#include<vector>
using namespace std;

int main() {
    vector<int> a(5,1); //(size, value)
    cout<<"vector a ";
    for(int i:a){
        cout<<i<<" ";
    }cout<<endl;    

    cout<<"vector last ";
    vector<int> last(a); // all the values of vector a are copied
    for(int i:last){
        cout<<i<<" ";
    }cout<<endl;     


    vector<int> v; //initialized with 0 memory
    cout<<"capacity of vector v: "<<v.capacity()<<endl;

    v.push_back(1);//push_back puts element in the back 
    cout<<"capacity of vector v: "<<v.capacity()<<endl;

    v.push_back(2);
    cout<<"capacity of vector v: "<<v.capacity()<<endl;

    v.push_back(3);
    cout<<"length of vector v: "<<v.size()<<endl; //Returns the number of elements in the vector.
    cout<<"capacity of vector v: "<<v.capacity()<<endl;    

    v.push_back(4);
    cout<<"capacity of vector v: "<<v.capacity()<<endl;

    v.push_back(5);
    cout<<"capacity of vector v: "<<v.capacity()<<endl;
    v.push_back(100);
    cout<<"capacity of vector v: "<<v.capacity()<<endl;

    cout<<"diff between first and last index"<<v.begin()-v.end()<<endl; 

    cout<<"elements at 2nd Index "<<v.at(2)<<endl;

    cout<<"front "<<v.front()<<endl;
    cout<<"back "<<v.back()<<endl;

    cout<<"before pop"<<endl;
    for(int i:v){
        cout<<i<<" ";
    }cout<<endl;

    v.pop_back();//pop_back removes element from the back 

    cout<<"after pop"<<endl;
    for(int i:v){
        cout<<i<<" ";
    }cout<<endl;    

    cout<<"before clear size "<<v.size()<<endl;
    v.clear();//size becomes 0 , but the capacity remains same
    cout<<"after clear size "<<v.size()<<endl;

    v.empty();

    v.insert(v.begin() + 2, 25);  // Inserts 25 at index 2
    v.erase(v.begin() + 1);  // Removes the element at index 1
    v.resize(10);  // Resizes the vector to contain 10 elements



    return 0;
}

/*

Time Complexity
Access Element (Random Access): O(1) 
Insert/Remove at End (push_back/pop_back): Amortized O(1) 
Insert/Remove at Beginning or Middle: O(n)
Iterating Over Elements: O(n)




*/
#include<iostream>
#include<list>
using namespace std;

//list is implemented by doubly linked list
//it has two pointers in the front and in the back



int main() {

    list<int> l;

    l.push_back(1);
    l.push_front(2); 

    for(int i:l){
        cout<<i<<" ";
    }cout<<endl;

    list<int> n(l); //copying contents of l to n

    list<int> h(5,100);//it has 5 elements all initialized by 100
    for(int i:h){
        cout<<i<<" ";
    }cout<<endl;

    l.begin();
    l.end();
    l.empty();
    l.front();
    l.back();


    l.erase(l.begin()); //it has time complexity of O(n) as we need to travel to each element to remove it

    for(int i:l){
        cout<<i<<" ";
    }cout<<endl<<"size of list "<<l.size()<<endl;

    
    return 0;
}
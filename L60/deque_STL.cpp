#include<iostream>
#include<deque>
using namespace std;
//https://en.cppreference.com/w/cpp/container/deque.html

int main() {

    deque<int> d;

    d.push_front(12);
    d.push_back(14);

    cout<<"front " <<d.front()<<endl;
    cout<<"back "<<d.front()<<endl;

    d.pop_front();

    cout<<"front " <<d.front()<<endl;
    cout<<"back "<<d.front()<<endl;

    d.pop_back();
    d.empty()?cout<<"queue is empty":cout<<"queue isnt empty";
    

    return 0;
}  

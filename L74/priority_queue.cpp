#include<iostream>
#include<queue>
using namespace std;

int main() {

    //maxheap
    priority_queue<int> pq;

    pq.push(4);
    pq.push(2);
    pq.push(5);
    pq.push(3);

    cout<<pq.top()<<endl;
    pq.pop();
    cout<<pq.top()<<endl;
    cout<<pq.size()<<endl;

    cout<<pq.empty()<<endl<<endl;

    //minheap
    priority_queue<int,vector<int>,greater<int>> minheap;
    minheap.push(4);
    minheap.push(2);
    minheap.push(5);
    minheap.push(3);

    cout<<minheap.top()<<endl;
    minheap.pop();
    cout<<minheap.top()<<endl;
    cout<<minheap.size()<<endl;

    cout<<minheap.empty();
    
    return 0;
}
//related to heap

#include<iostream>
#include<queue>
using namespace std;

int main() {

    //max heap (The element with the highest value is at the front.)
    priority_queue<int> maxi; //default initialization is of max heap

    //min heap(The element with the lowest value is at the front.)
    priority_queue<int,vector<int>,greater<int>> mini;
    
    maxi.push(1);
    maxi.push(3);    
    maxi.push(2);
    maxi.push(0);

    cout<<"size : "<<maxi.size()<<endl;
    int n= maxi.size();
    for(int i=0;i<n;i++){
        cout<<maxi.top()<<" ";
        maxi.pop();
    }cout<<endl;

    mini.push(5);
    mini.push(1);    
    mini.push(0);
    mini.push(4);
    mini.push(3);

    cout<<"size : "<<mini.size()<<endl;
    int m= mini.size();
    for(int i=0;i<m;i++){
        cout<<mini.top()<<" ";
        mini.pop();
    }cout<<endl;

    cout<<mini.empty();

    return 0;
}
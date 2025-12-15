#include<iostream>
#include<queue>
using namespace std;

int main() {

    queue<int> num;

    num.push(0);
    num.push(1);
    num.push(2);
    num.push(3);
    
    int front = num.front();
    int back = num.back();
    int size =num.size();

    cout<<"front "<<front << ", back "<<back<< ", size " <<size<<endl;
    
    num.empty() ? cout<<"queue is empty":cout<<"queue isnt empty";


    cout<<endl<<"size is "<<num.size()<<endl; 

    for(;!num.empty(); num.pop()){
        cout<<num.front()<<" ";
    }
    cout<<endl;
    num.empty() ? cout<<"queue is empty":cout<<"queue isnt empty";
    
    cout<<endl<<"size is "<<num.size()<<endl;
    
    return 0;
}
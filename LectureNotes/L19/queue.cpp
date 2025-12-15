//A queue is a linear data structure that follows the 
//FIFO (First In, First Out) principle. This means that the first element 
//added to the queue will be the first one to be removed, 
//much like a real-world queue 


#include<iostream>
#include<queue>
using namespace std;

int main() {

    queue<string> q;

    q.push("love");
    q.push("babbar");
    q.push("kumar");

    cout<<"size before pop "<<q.size()<<endl;

    cout<<"first element "<<q.front()<<endl;
    q.pop();
    cout<<"first element "<<q.front()<<endl;
    
    cout<<"size after pop "<<q.size()<<"\n\n\n\n";

    queue<int> num;
    num.push(0);
    num.push(1);
    num.push(2);
    num.push(3);
    
    int front = num.front();
    int back = num.back();
    int size =num.size();

    cout<<"front "<<front << ", back "<<back<< ", size " <<size<<endl;
    
    cout<<"size is "<<num.size()<<endl;

    for(;!num.empty(); num.pop()){
        cout<<num.front()<<" ";
    }
    cout<<endl<<"size is "<<num.size()<<endl;


 

    return 0;
}
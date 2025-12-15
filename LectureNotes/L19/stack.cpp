//A stack is a linear data structure that follows the 
//LIFO (Last In, First Out) principle. This means that the last element 
//added to the stack is the first one to be removed.

#include<iostream>
#include<stack>
using namespace std;

int main() {
    stack<string> s;

    s.push("love");
    s.push("babbar");
    s.push("kumar");

    cout<<"top element "<<s.top()<<endl;
    s.pop();
    cout<<"top element "<<s.top()<<endl;

    cout<<"size of stack "<<s.size()<<endl;
    s.empty();
    
    return 0;
}
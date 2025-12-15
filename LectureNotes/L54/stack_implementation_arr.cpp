//A stack is a linear data structure that follows the 
//LIFO (Last In, First Out) principle. This means that the last element 
//added to the stack is the first one to be removed.

#include<iostream>
//#include<stack>
using namespace std;


class stack{
    //properties
    public:
    int* arr;
    int size;
    int top;


    //behaviour
    stack(int size){//time complexity O(1)
        this->size=size;
        arr = new int[size];
        top=-1;
    }

    void push(int element){//time complexity O(1)
        if(size-top>1){
            top++;
            arr[top]=element; 
        }else{
            cout<<"stack overflow "<<endl;
        }
    }

    void pop(){//time complexity O(1)
        if(top>=0){
            top--;
        }else{
            cout<<"stack underflow"<<endl;
        }

    }

    int peek(){//time complexity O(1)
        if(top>=0){
            return arr[top];
        }else{
            cout<<"stack is empty"<<endl;
            return -1;
        }
    }

    bool isEmpty(){//time complexity O(1)
        if(top==-1){
            return true;
        }else{
            return false;
        }
    }

    bool isFUll(){
        if(top == size-1) return true;
    }

};




int main() {

    stack st(5);

    st.push(22);
    st.push(43);
    st.push(44);
    st.push(22);
    st.push(43);
    st.push(44);
    

    cout<<st.peek()<<endl;

    st.pop();
    cout<<st.peek()<<endl;

    st.pop();
    cout<<st.peek()<<endl;

    st.pop();
    cout<<st.peek()<<endl;

    if(st.isEmpty()){
        cout<<"stack is empty mere dost"<<endl;
    }else{
        cout<<"stack is not empty mere dost"<<endl;
    }












    // stack<string> s;

    // s.push("love");
    // s.push("babbar");
    // s.push("kumar");

    // cout<<"top element "<<s.top()<<endl;
    // s.pop();
    // cout<<"top element "<<s.top()<<endl;

    // cout<<"size of stack "<<s.size()<<endl;
    // s.empty();


    // //creation of stack
    // stack<int> s;

    // //push operation
    // s.push(2);
    // s.push(3);

    // //pop
    // s.pop();


    // cout<<"printing top element "<<s.top()<<endl;

    // if(s.empty()){
    //     cout<<"stack is empty"<<endl;
    // }else{
    //     cout<<"stack is not empty"<<endl;
    // }

    // cout<<" size of stack is "<<s.size()<<endl;




    return 0;
}
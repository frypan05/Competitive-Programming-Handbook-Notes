//https://www.naukri.com/code360/problems/two-stacks_983634

#include<iostream>
//#include<stack>
using namespace std;


class TwoStack{

    public:
    int* arr;
    int size;
    int top1;
    int top2;


    TwoStack(int size){
        this->size=s;
        
        top1=-1; //stack1 insertion left to right
        top2 = s; //stack2 insertion right to left
        arr = new int[s]; 
    }

    //push in stack 1
    void push1(int num){
        if(top2-top1>1){    
            top1++;
            arr[top1]=num; 
        }else{
            cout<<"stack overflow "<<endl;
        }
    }
    
    //push in stack 2
    void push2(int num){
        if(top2-top1>1){
            top2--;
            arr[top2]=num; 
        }else{
            cout<<"stack overflow "<<endl;
        }
    }


    //pop from stack 1 and return popped element
    int pop1(){
        if(top1>=0){
            int ans = arr[top1];
            top1--;
            return ans;
        }else{
            return -1;
        }

    }

    //pop from stack 2 and return popped element
    int pop2(){
        if(top2<size){
            int ans = arr[top2];
            top2++;
            return ans;
        }else{
            return -1;
        }

    }



};




int main() {


    return 0;
}
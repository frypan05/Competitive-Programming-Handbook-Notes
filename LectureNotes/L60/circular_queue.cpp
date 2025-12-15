#include<bits/stdc++.h>
using namespace std;
https://www.naukri.com/code360/problems/queue-using-array-or-singly-linked-list_2099908

circularQueue{
    public:
    int* arr;
    int front;
    int rear;
    int size;

    circularQueue(int n){
        size = n ;
        arr = new int[size];
        front = rear = -1;
    }

    bool enqueue(int value){
        if((front ==0 && rear==size-1) || (rear==(front-1)%(size-1))){ //to chcek if queue is full
            //cout<<'queue is full';
            return false;
        }else if(front == -1){ //first element to push
            front = rear = 0;
        }else if(rear==size-1 && front !=0){ //to maintain cyclic nature
            rear=0; 
        }else{//normal flow
            rear++;
        }
        arr[rear] = value;
        return true;
    }

    int dequeue(){
        if(front==-1){ //to check if empty
            //cout<<"queue is empty";
            return -1;
        }
        int ans = arr[front];
        arr[front]=-1;

        if(front == rear){
            front = rear = -1;
        }else if(front == size-1){//to maintain cyclic nature
            front =0; 
        }else{//normal flow
            front ++;
        }      
        return ans;
    }

};

int main() {
    
    return 0;
}
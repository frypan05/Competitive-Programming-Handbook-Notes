#include<bits/stdc++.h>
using namespace std;

class deque{
    public:
    int *arr;
    int front;
    int rear;
    int size;

    deque(int n){
        size = n;
        arr = new int[size];
        front = -1;
        rear = -1;
    }

    bool pushFront(int x){
        if(isFull()){
            return false;
        }else if(isEmpty()){
            front =rear=0;
        }
        // else if( front ==0 && rear== size-1){ flawed
        //     front = size-1;
        // }else{
        //     front--;
        // }
        else{
            front = (front - 1 + size) % size;
        }
        arr[front]=x;
        return true;
    }

    bool pushRear(int x){
        if(isFull()){
            return false;
        }else if(isEmpty()){
            front =rear=0;
        }
        else if(rear== size-1 && front!=0){ flawed
            rear = 0;
        }else{
            rear++;
        }
    
        arr[rear]=x;
        return true;
    }

    
    int popFront(){ //same as dequeue
        if(isEmpty()){ //to check if empty
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
    int popRear(){
        if(isEmpty()){ //to check if empty
            //cout<<"queue is empty";
            return -1;
        }
        int ans = arr[rear];
        arr[rear]=-1;

        if(front == rear){
            front = rear = -1;
        }else if(rear==0){//to maintain cyclic nature
            rear =size-1; 
        }else{//normal flow
            rear --;
        }      
        return ans;
    }

    
    int getFront(){
        if(isEmpty()){
            return -1;
        }
        return arr[front];
    }

    int getRear(){
        if(isEmpty()){
            return -1;
        }
        return arr[rear];
    }

    
    bool isEmpty(){
        if(front ==-1 ){
            return true;
        }else{
            return false;
        }
    }

    
    bool isFull(){
        if((front==0 && rear == size-1) ||(front !=0 && rear== (front-1)%(size-1))){
            return true;
        }else{
            return false;   
        }
    } 
    
};

int main() {
    
    return 0;
}

//alternate approach
// #include <bits/stdc++.h> 
// class Deque
// {

// public:
//     int *arr;
//     int front;
//     int rear;
//     int size;
//     // Initialize your data structure.
//     Deque(int n)
//     {
//         size = n;
//         arr = new int[size];
//         front = -1;
//         rear = -1;
//     }

//     // Pushes 'X' in the front of the deque. Returns true if it gets pushed into the deque, and false otherwise.
//     bool pushFront(int x)
//     {
//         if(isFull()){
//             return false;
//         }else if(isEmpty()){
//             front =rear=0;
//         }else{
//             front = (front - 1 + size) % size;
//         }
//         arr[front]=x;
//         return true;
//     }

//     // Pushes 'X' in the back of the deque. Returns true if it gets pushed into the deque, and false otherwise.
//     bool pushRear(int x)
//     {
//         if(isFull()){
//             return false;
//         }else if(isEmpty()){
//             front =rear=0;
//         }else {
//             // The correct way to handle a circular increment
//             rear = (rear + 1) % size;
//         }
//         arr[rear]=x;
//         return true;
//     }

//     // Pops an element from the front of the deque. Returns -1 if the deque is empty, otherwise returns the popped element.
//     int popFront()
//     {
//         if(isEmpty()){ //to check if empty
//             return -1;
//         }
//         int ans = arr[front];
//         arr[front]=-1;

//         if(front == rear){
//             front = rear = -1;
//         }else {
//             front = (front + 1) % size;
//         }
//         return ans;
//     }

//     // Pops an element from the back of the deque. Returns -1 if the deque is empty, otherwise returns the popped element.
//     int popRear()
//     {
//         if(isEmpty()){ //to check if empty
//             return -1;
//         }
//         int ans = arr[rear];
//         arr[rear]=-1;

//         if(front == rear){
//             front = rear = -1;
//         }else {
//             rear = (rear - 1 + size) % size;
//         }
//         return ans;
//     }

//     // Returns the first element of the deque. If the deque is empty, it returns -1.
//     int getFront(){
//         return isEmpty()? -1: arr[front];
//     }

//     // Returns the last element of the deque. If the deque is empty, it returns -1.
//     int getRear(){
//         return isEmpty() ?  -1:arr[rear];
//     }

//     // Returns true if the deque is empty. Otherwise returns false.
//     bool isEmpty(){
//         return front ==-1 ?true: false;
//     }

//     // Returns true if the deque is full. Otherwise returns false.
//     bool isFull(){
//         // This is the standard and correct logic for a circular array to be full.
//         // It leaves one empty slot to distinguish from an empty state.
//         return (rear + 1) % size == front;
//     }
// };
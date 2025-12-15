#include<bits/stdc++.h>
using namespace std;
//https://www.naukri.com/code360/problems/queue-using-array-or-singly-linked-list_2099908

class queue{
    public:
    int* arr;
    int qfront;
    int rear;
    int size;

    queue(){
        size = 100001;
        arr = new int[size];
        qfront =0;
        rear =0;
    }

    bool isEmpty(){
        return qfront==rear ?true: false;
    }
    
    bool isFull()
    {
        return rear == size;
    }

    void enqueue(int data){
        if(rear==size)
            cout<<'queue is full';
        else{
            arr[rear]=data;
            rear++;
        }
    }

    int dequeue(){
        if(qfront==rear)
            return -1;
        else{
            int ans = arr[qfront];
            arr[qfront] = -1
            qfront++;
            if(qfront==rear){
                qfront=0;
                rear=0;
            }
        }
        return ans;
    }

    int front(){
        if(qfront==rear)
            return -1;
        else{
            return  arr[qfront];
        }
    }

    int rear(){
        if(qfront==rear)
            return -1;
        else{
            return  arr[rear-1];
        }
    }

};

int main() {
    
    return 0;
}
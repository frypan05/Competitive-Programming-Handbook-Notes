#include<bits/stdc++.h>
using namespace std;

class Node{
    public:
    int data;
    Node* next; 

    //constructor
    Node(int data){
        this->data=data;
        this->next=NULL;
    }

    //destructor
    ~Node(){
        int value = this->data;
        //memory free
        if(this->next!=NULL){
            delete next;
            this->next=NULL;
        }
        cout<<" memory is free for node with data "<<value<<endl;
    }

};

Node* getMiddle(Node* head){
    if( head==NULL || head ->next==NULL){
        return head;
    }

    //2 nodes - needed or not?
    if(head->next->next=NULL){
        return head->next;
    }

    Node*slow=head;
    Node*fast=head->next;

    while(fast!=NULL){
        fast = fast->next;
        if(fast!=NULL){
            fast = fast->next;
        }
        slow=slow->next;
    }
    return slow;
}



int main() {
    
    return 0;
}
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

//it will return head of the reversed list
Node* reverse1(Node* head){
    //base case
    if(head == NULL /*case of empty list*/|| head->next==NULL/*case of single node */){
        return head;
    }

    Node* chotaHead = reverse1(head->next);
    head->next->next=head;
    head->next=NULL; 
    return chotaHead;
} 

Node* reverseLinkedList(Node* head){
    return reverse1(head);
}

Node* reverseLinkedList1(Node* head){
    if(head == NULL /*case of empty list*/|| head->next==NULL/*case of single node */){
        return head;
    }

    Node* rest = reverseLinkedList1(head->next);
    head->next->next=head;
    head->next=NULL; 
    return rest;
}

 

int main() { 
    
    return 0;
}
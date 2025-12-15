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



void reverse(Node* &head,Node*curr,Node* prev){
    //base case
    if(curr==NULL){
        head=prev;
        return;
    }

    Node* forward =curr->next;
    reverse(head,forward,curr);
    curr->next = prev;
}



Node* reverseLinkedList(Node* head){

    Node* curr=head;
    Node* prev= NULL;
    reverse(head,curr,prev);
    return head;


}

int main() { 
    
    return 0;
}
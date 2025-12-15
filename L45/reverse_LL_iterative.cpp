#include<bits/stdc++.h>
using namespace std;

//https://www.youtube.com/redirect?event=video_description&redir_token=QUFFLUhqazdjNjhxb18xc1VjSlZXWkxkMGo1Zkh4aEt2QXxBQ3Jtc0tuWGlvX3BPbFU5V045OGtmN01PWGtlMmRJeWk0ejJ3ODJ3aHVlTEZXamMyb3pOZ3ctWklCWlBNcEU3U3VRTnlMOHIxUkYwc0hyVUxxRm5PUTBSYi1kNGNoaVR0NHhXY3JtMXNRNUg5LVA2bFhvck5sOA&q=https%3A%2F%2Fbit.ly%2F3H1lKov&v=vqS1nVQdCJM

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


Node* reverseLinkedList(Node* head){

    if(head == NULL /*case of empty list*/|| head->next==NULL/*case of single node */){
        return head;
    }

    Node *prev = NULL;
    Node* curr = head;
    Node* forward=NULL;

    while(curr!=NULL){
        
        curr->next = prev;
        prev=curr;
        curr=forward;
    }

    return prev;
}

void reverse(Node* head ){
    while(curr!=NULL){
        
        forward = curr->next;
        curr->next = prev;
        prev=curr;
        curr=forward;
    }
    head = prev;
}

int main() {
    
    return 0;
}
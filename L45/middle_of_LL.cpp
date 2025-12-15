#include<bits/stdc++.h>
using namespace std;

//https://www.naukri.com/code360/problems/middle-of-linked-list_973250?source=youtube&campaign=Lovebabbarcodestudio_24thJan&utm_source=youtube&utm_medium=affiliate&utm_campaign=Lovebabbarcodestudio_24thJan

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

int getLength(Node* head){//O(n)
    int len=0;
    while(head!=NULL){
        len++;
        head=head->next;
    }
    return len; 
}

Node * findMiddle(Node* head){//O(n/2)
    int len=getLength(head);
    int ans= (len/2);
    Node* temp=head;
    int cnt=0; 
    while(cnt<ans){
        temp=temp->next;
        cnt++; 
    }
    return temp;
}

int main() { 
    
    return 0;
}
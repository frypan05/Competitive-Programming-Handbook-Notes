//https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/description/

#include<bits/stdc++.h>
using namespace std;

class Node{
    public:
    int data;
    Node* next;

    Node(int data){
        this->data=data;
        this->next=NULL;
    }

};

void insertAtTail(Node*tail,int data){
    Node* temp = new Node(10);
    tail -> next = temp;
    tail = temp;   
}

void print(Node* head){
    Node* temp = head;
    while(temp!=NULL){
        cout<<temp->data<<" ";
        temp= temp->next;
    }
    cout<<endl;
}

Node* removeDupes_Sorted(Node* head){
    if(head==NULL){
        return NULL;
    }

    Node* temp = head;

    while(temp!= NULL){
        if( (temp->next!=NULL) && (temp->data== temp->next->data) ){ //beware short circuiting issue, condn checks from L to R , => interchanging the order of condn will cause runtime error
            Node* next_next = temp->next->next;
            Node* nodeToDelete = temp->next;
            delete(nodeToDelete);
            temp->next = next_next;
        }else{    
            temp=temp->next;  
        }
    }
    return head;
}


int main() {

    Node* node1 = new Node(10);
    Node*head = node1;
    Node*tail = node1;
    insertAtTail(tail,20);
    

    return 0;
}
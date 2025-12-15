#include<bits/stdc++.h>
using namespace std;

//https://www.naukri.com/code360/problems/reverse-list-in-k-groups_983644?leftPanelTab=0%3Fsource%3Dyoutube&campaign=Lovebabbar_codestudio_26thjan

class Node{
    public:
        int data;
        Node* next;

        Node(int data){
            this->data=data;
            this->next=NULL;
        }

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

Node* kReverse(Node* head, int k) {
    
    //base case
    if(head==NULL){
        return NULL;
    }
    
    // Check if there are at least k nodes remaining
    Node* currentCheck = head;
    int checkCount = 0;
    while(currentCheck != NULL && checkCount < k){
        currentCheck = currentCheck->next;
        checkCount++;
    }
    // If less than k nodes are remaining, do not reverse them
    if(checkCount < k){
        return head;
    }

    //step1: reverse first k nodes
    Node* next=NULL;
    Node* curr=head;
    Node* prev=NULL;
    int count=0;

    while(curr!=NULL && count<k){
        next = curr->next;
        curr -> next = prev;
        prev=curr;
        curr=next;
        count++;
    }

    //step2 : recusion
    if(next!=NULL){
        head->next = kReverse(next,k);
    }

    //step3: return head of reversed list
    return prev;

}

int main() {
    
    return 0;
}
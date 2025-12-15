//https://www.naukri.com/code360/problems/sort-linked-list-of-0s-1s-2s_1071937

#include<bits/stdc++.h>
using namespace std;

class Node{
public:
    int data;
    Node *next;
    
    Node(int data)
    {
        this->data = data; 
        this->next = NULL;
    }
    
};

//approach 1 - store count
Node* sortList(Node *head){
    Node* temp =head;

    int zeroCount = 0;
    int oneCount = 0;
    int twoCount = 0;

    while(temp!=NULL){
        if(temp->data==0)
            zeroCount++;
        else if(temp->data==1)
            oneCount++;
        else if(temp->data==2)
            twoCount++;
        
        temp = temp->next;
    }
    temp = head;
    while(temp!=NULL){
        if(zeroCount!=0){
            temp->data=0;
            zeroCount--;
        }else if(oneCount!=0){
            temp->data =1;
            oneCount--;
        }else if(twoCount!=0){
            temp->data=2;
            twoCount--;
        }

        temp = temp->next;
            
    } 
    return head;
}

//--------------------------------------------------


void insertAtTail(Node* &tail,Node*curr){

    tail->next = curr;
    tail = curr;
}
 
//approach 2 - modifying links b/w nodes
Node* sortList(Node* head){

    //dummy nodes for 0,1,2
    Node* zeroHead = new Node(-1);
    Node* zeroTail = zeroHead;
    Node* oneHead = new Node(-1);
    Node* oneTail = oneHead;
    Node* twoHead = new Node(-1);
    Node* twoTail = twoHead;
    
    Node* curr = head;

    //create separte lists
    while(curr!=NULL){
        int value = curr->data;

        if(value==0){
            insertAtTail(zeroTail,curr);
        }else if(value==1){
            insertAtTail(oneTail,curr);
        }else if(value==2){
            insertAtTail(twoTail,curr);
        }

        curr = curr->next;
    }

    //merge 3 sublists 
    if(oneHead->next!=NULL){//1s list not empty
        zeroTail->next= oneHead->next;
    }else{//1s list empty
        zeroTail->next = twoHead->next;
    }

    oneTail->next = twoHead->next;
    twoTail->next = NULL;

    //setup head
    head = zeroHead->next;

    //delete dummy nodes
    delete zeroHead;
    delete oneHead;
    delete twoHead;

    return head;
}

int main() {
    
    return 0;
}
//https://www.geeksforgeeks.org/problems/clone-a-linked-list-with-next-and-random-pointer/1

#include<bits/stdc++.h>
using namespace std;

class Node {
    public:
    int data;
    Node *next;
    Node *random;

    Node(int x) {
        data = x;
        next = NULL;
        random = NULL;
    }
};

//approach 1:using map 

void insertAtTail(Node*&head , Node*&tail,int d){
    Node* newNode = new Node(d);
    if(head==NULL){
        head= newNode;
        tail = newNode;
        return;
    }else{
        tail->next = newNode;
        tail = newNode;
    }
}

Node* copyList(Node* head){
    //step1: create a clone list
    Node* cloneHead = NULL;
    Node* cloneTail = NULL;

    Node* temp = head;
    while(temp!=NULL){
        insertAtTail(cloneHead,cloneTail,temp->data);
        temp= temp->next;
    }

    //step2: create a map
    unordered_map<Node*,Node*> oldToNewNode;

    Node* originalNode = head;
    Node* cloneNode = cloneHead;
    while(originalNode!=NULL && cloneNode!=NULL){
        oldToNewNode[originalNode]=cloneNode;
        originalNode = originalNode->next;
        cloneNode = cloneNode->next;
    }

    originalNode = head;
    cloneNode = cloneHead;
    while(originalNode!=NULL){
        cloneNode->random = oldToNewNode[originalNode->random];
        originalNode = originalNode->next;
        cloneNode = cloneNode->next;  
    } 

    return cloneHead;

}   

//-----------------------------------------------------------
//approach 2 : chaining links
Node* copyNode(Node* head){

    //step1: create a clone list
    Node* cloneHead=NULL;
    Node* cloneTail = NULL;
    Node* temp = head;

    while(temp!=NULL){
        insertAtTail(cloneHead,cloneTail,temp->data);
        temp=temp->next;
    }

    //step2: cloneNodes add in between original list
    Node* originalNode = head;
    Node* cloneNode = cloneHead;

    while(originalNode!=NULL && cloneNode!=NULL){
        Node* next = originalNode -> next;
        originalNode->next = cloneNode;
        originalNode = next;

        next = cloneNode->next;
        cloneNode ->next = originalNode;
        cloneNode = next;
    }

    //step3: random pointer copy
    temp = head;
    while(temp!=NULL){
        if(temp->next!=NULL){
            // if(temp->random!=NULL){
            //     temp->next->random = temp->random->next;
            // }else{
            //     temp->next = NULL;//temp->next= temp->random; 
            // }
            temp->next->random = temp->random ? temp->random->next : NULL
        }
        temp = temp->next->next;
    }

    //step4: revert changes done in step2
    originalNode = head;
    cloneNode = cloneHead;
    
    while(originalNode!=NULL && cloneNode!=NULL){
        originalNode->next = cloneNode->next;
        originalNode = orginalNode->next;

        if(originalNode!=NULL){
            cloneNode->next = originalNode->next;//possible null pointer
            cloneNode = cloneNode->next;
        }
    }

    //step5: return ans
    return cloneHead;
} 

 
int main() {
    
    return 0;
}
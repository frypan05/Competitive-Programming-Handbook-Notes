#include<bits/stdc++.h>
using namespace std;



class Node{
    public:
    int data;
    Node * next;

    Node(int data){
        this->data = data;
        this->next= NULL;
    }


};

void print(Node*tail){
    Node* temp = tail;
    temp = temp->next;
    while(temp != tail or temp!= NULL){
        cout<<temp->data;
        temp = temp->next;
    }

}

void insertNode(Node*tail,int data , int position){

}

//using loop
bool isCircular(Node* head){
    
    //empty list
    if(head==NULL){
        return true;
    }

    //non empty
    Node *temp = head->next;
    while(temp!=NULL && temp!= head){
        temp = temp->next;
    }
    
    if(temp==head)
        return true;

        
    return false;
}

//uisng map
bool isCircular_map(){
    map<Node* , bool> visited;
}


int main() {
    Node* tail = NULL;

    insertNode(tail,5,3);
    print(tail);
    insertNode(tail,3,5);
    print(tail);
    
    if(isCircular(tail)){
        cout<<"LL is circular";
    }else{
        cout<<"LL is not circular";
    }

    return 0;
}
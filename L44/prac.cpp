#include<bits/stdc++.h>
using namespace std;

class node{
    public:
    int data;
    node*next;

    node(int data){
        this->data=data;
        this->next= NULL;
    }

};

void insertattail(int data, node*tail){
    node*temp = new node(data);
    tail->next = temp;
    temp = tail; 

}

void print( node * head){
    node* temp = head;

    while(temp!=NULL){
        cout<< temp->data<<" ";
        temp = temp->next;
    }
}

int main() {

    node*node1 = new node(10);
    node*head = node1;
    node*tail = node1;
    insertattail(20,head);
    

    

    print(head);

    
    return 0;
}
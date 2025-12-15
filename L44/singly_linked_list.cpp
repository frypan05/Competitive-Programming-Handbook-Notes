#include<bits/stdc++.h>
using namespace std;

class node{
    public:
    int data;
    node* next; 

    //constructor
    node(int data){
        this->data=data;
        this->next=NULL;
    }

    //destructor
    ~node(){
        int value = this->data;
        //memory free
        if(this->next!=NULL){
            delete next;
            this->next=NULL;
        }
        cout<<" memory is free for node with data "<<value<<endl;
    }

};

void insertAtHead(node* &head,int d){

    //new node create
    node* temp = new node(d);
    temp->next=head;
    head=temp;
}

void InsertAtTail(node* & tail, int d){
    //new node create
    node* temp = new node(d);
    tail->next=temp;
    tail= tail-> next;  // tail= temp;

}

void print(node* &head){
    node* temp =head;

    while(temp!=NULL){

        cout<<temp->data<<" ";
        temp=temp->next;

    } 
    cout<<endl;

}

void insertAtPosition(node* &tail,node* &head,int position,int d){
    //insert at start
    if(position==1){
        insertAtHead(head,d);
        return;
    }

    node* temp = head;
    int cnt=1;
    while(cnt<position-1){
        temp = temp->next;
        cnt++;
    }

    //insertig at last position
    if(temp->next==NULL){
        InsertAtTail(tail,d);
        return;

    }


    //creating a node for d
    node* nodeToInsert = new node(d);

    nodeToInsert-> next  = temp->next;
    temp->next=nodeToInsert;

}

void deleteNode(int position, node* &head){
    if(position==1){

        node* temp = head;
        head=head->next;
        //memory free start node
        temp->next=NULL;
        delete temp;

    }else{

        //deleting any middle node or last node
        node* current = head;
        node* previous =NULL;
        int cnt =1;
        while(cnt<position){
            previous = current;
            current = current->next;
            cnt++;  
        }
        previous->next=current->next;
        current->next=NULL;
        delete current;

    }
}
bool isCircular(node* head){
    
    //empty list
    if(head==NULL){
        return true;
    }

    //non empty
    node *temp = head->next;
    while(temp!=NULL && temp!= head){
        temp = temp->next;
    }
    
    if(temp==head)
        return true;

        
    return false;
}

int main() {

    //created a new node
    node* node1 =  new node(10);
    //cout<<node1-> data<<endl;
    //cout<<node1-> next<<endl;


    //head pointed to node 1
    node* head =node1;
    node* tail=node1;
    print(head);

    //insertAtHead(head,12);
    InsertAtTail(tail,12);

    print(head);

    //insertAtHead(head,15);
    InsertAtTail(tail,15);
    print(head);


    //middle
    //insertAtPosition(head,3,22);
    //print(head);

    //head
    //insertAtPosition(head,1,22);
    //print(head);

    //tail
    insertAtPosition(tail,head,4,22);
    print(head);
    cout<<"head "<<head->data<<endl;
    cout<<"tail "<<tail->data<<endl;   

    if(isCircular(tail)){
        cout<<"yes"<<endl;
    }else{
        cout<<"no"<<endl;
    }

    deleteNode(4,head);
    print(head);

    
    
    return 0;
}
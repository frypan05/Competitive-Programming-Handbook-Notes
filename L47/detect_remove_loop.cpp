//https://www.naukri.com/code360/problems/interview-shuriken-42-detect-and-remove-loop_241049

#include<bits/stdc++.h>
using namespace std;

class Node{
    public:
    int data;
    Node* next;

    Node(int data){
        this->data = data ;
        this->next = NULL;
    }
};

void print(Node* head){
    Node* temp = head;
    while(temp!= NULL){
        cout<<temp->data<<" ";
        temp = temp->next;
    }
}
void insertAtHead(Node* &head,int d){

    //new node create
    Node* temp = new Node(d);
    temp->next=head; 
    head=temp;
}

void insert(Node* tail,int data,int position){

}

//approach 1 hash map
bool detectLoop(Node * head){
    if(head ==NULL){
        return false;
    }
    map<Node*,bool> visited;
    Node* temp = head;

    while(temp!= NULL){
        
        //cycle is present
        if(visited[temp]== true){
            cout<<" Present on element "<< temp->data<<endl;
            return true;
        }

        visited[temp] = true;
        temp = temp->next; 
    }
    return false; 
}

//approach 2 - floyd cycle detection
Node* floydDetectLoop(Node* head){

    if(head ==NULL){
        return NULL;
    }

    Node *slow = head;
    Node *fast = slow;

    while(slow != NULL && fast!= NULL){
        fast = fast->next;
        if(fast!=NULL){
            fast = fast->next;
        }
        slow = slow->next;
        if(slow==fast){ //loop exists
            cout<<"presnt at "<<slow->data<<endl; //address of point of intersection
            return slow;
        }
    }
    return NULL;
}

//approach 3: FCD modified for cycle start detection
Node* getStartingNode(Node* head){
    if(head==NULL){
        return NULL;
    }

    Node* intersection =floydDetectLoop(head);
    if(intersection==NULL){
        return NULL;
    }
    Node*slow =head;
    while(slow!= intersection){
        slow= slow->next;
        intersection=intersection->next;
    }
    return slow; //address of start of loop

}

//approach 4: break the loop after detecting  

Node *removeLoop(Node *head)
{
    if(head==NULL){
        return NULL;
    }
    Node* startOfLoop = getStartingNode(head);
    if(startOfLoop==NULL){
        return head;
    }
    Node*temp = startOfLoop;
    while(temp->next!= startOfLoop){
        temp = temp->next;
    }

    temp->next=NULL;
    return head;
}

int main() {

    Node* node1 =  new Node(10);
    Node* head = node1;
    Node* tail = node1;

    insertAtHead(head,20);
    insertAtHead(head,30);
    insertAtHead(head,40);

    tail->next = head->next;
    

    print(head);


    return 0;
}
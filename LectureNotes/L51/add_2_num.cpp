//https://www.geeksforgeeks.org/problems/add-two-numbers-represented-by-linked-lists/1

#include<bits/stdc++.h>
using namespace std;

template <typename T>
class Node{
    
    public:
    T data;
    Node* next;

    Node(T data){
        this->data=data;
        this->next=NULL;
    }
};
Node<int>* reverse(Node<int>* head){

    if(head == NULL|| head->next==NULL){
        return head;
    }

    Node *prev = NULL;
    Node* curr = head;
    Node* forward=NULL;

    while(curr!=NULL){
        forward = curr->next;
        curr->next = prev;
        prev=curr;
        curr=forward;
    }

    return prev;

}

void insertAtTail(Node<int>* &head,Node<int>* &tail, int data){
    Node<int>* temp = new Node(data);
    if(head==NULL){//if list is empty
        head= temp;
        tail=temp;
        return;
    }else{//non empty list
        tail->next = temp;
        tail = temp;
    }
}

Node<int>* addNodes(Node<int>* first, Node*<int> second){
    int carry =0;

    Node<int>* ansHead = NULL;
    Node<int>* ansTail =NULL;

    while(first!=NULL || second!=NULL || carry !=0){

        int val1 = 0;
        if(first!=NULL){
            val1 = first->data;
        }

        int val2 = 0;
        if(second!=NULL){
            val2 = second->data;
        }

        int sum = carry + val1 + val2;
        int digit = sum%10;
        //create node and add in ans LL
        insertAtTail(ansHead,ansTail,digit);

        carry = sum/10;
        
        if(first!=NULL)
            first = first->next;
        
        if(second!=NULL)
            second = second ->next;
    }


    return ansHead;
}

Node<int>* addTwoLists(Node<int>* num1, Node<int>* num2){
    //step1 - reverse input LL
    first = reverse(num1);
    second = reverse(num2);

    //step2 - add the nodes
    Node* ans = addNodes(first,second);

    //step3 -reverse
    ans = reverse(ans);

    //step4 - handle leading zeros in the result
    // If the result is not just '0', remove leading zeros.
    while (ans != NULL && ans->next != NULL && ans->data == 0) {
        Node* temp = ans;
        ans = ans->next;
        delete temp; // Free memory of the removed node
    }
    
    return ans;
}

int main() {
    
    return 0;
}
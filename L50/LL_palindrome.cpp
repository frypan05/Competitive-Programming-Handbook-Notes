//https://www.geeksforgeeks.org/problems/check-if-linked-list-is-pallindrome/1

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

//approach 1: using array
bool isPalindrome(Node* head){
    vector<int> arr;
    Node*temp = head;
    while(temp!=NULL){
        arr.push_back(temp->data);
        temp = temp->next;
    }

    int s = 0;
    int e = arr.size()-1;
    while(s<=e){
        if(arr[s]==arr[e]){
            s++;
            e--;
        }else{
            return false;
        }
    }

    return true;

}

//approach 2: LL reversal from mid
Node* getMid(Node* head){//using slow and fast pointer to find mid
    Node* slow= head;
    Node* fast= head->next;

    while(fast!=NULL && fast->next!=NULL){
        fast=fast->next->next;
        slow=slow->next;
    }
    return slow; //mid
}

Node* reverse(Node* head){
    Node* curr = head;
    Node* prev = NULL;
    Node* next = NULL;

    while(curr!=NULL){
        next = curr->next;
        curr ->next = prev;
        prev = curr;
        curr = next;
       
    }
    return prev;
}

bool isPalindrome(Node* head){
    if(head==NULL or head->next=NULL){
        return true;
    }

    //step1-> find mid
    Node*middle = getMid(head);

    //step2 -> reverse list after mid
    Node* temp = middle->next;
    middle->next = reverse(temp);

    //step3 -> compare both halves
    Node* head1 = head;
    Node* head2 = middle->next;

    while(head2!=NULL){
        if(head1->data!=head2->data){
            return false;
        }
        head1= head1->next;
        head2= head2->next;
    }

    //step4-> repeat step2(optional)
    temp = middle->next;
    middle->next = reverse(temp);

    return true;
}


int main() {
    
    return 0;
}

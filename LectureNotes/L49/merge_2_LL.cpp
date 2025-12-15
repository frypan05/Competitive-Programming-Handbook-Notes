//https://www.naukri.com/code360/problems/merge-two-sorted-linked-lists_800332

#include<bits/stdc++.h>
using namespace std;

template <typename T>
class Node{
    public:
    T data;
    Node *next;
    
    Node(T data)
    {
        this->data = data; 
        this->next = NULL;
    }
    
};    

Node<int>* solve(Node<int>* first, Node<int>* second){//to check which list has smaller starting element
    
    //if only one element is present in first list, point its next to entire second list
    if(first->next ==NULL){
        first->next = second;
        return first;
    }
    
    
    Node<int>* curr1 = first;
    Node<int>* next1 = curr1->next;
    Node<int>* curr2 = second;
    Node<int>* next2 = curr2->next;

    while(next1!=NULL && curr2 !=NULL){
        if((curr2->data>=curr1->data) && (curr2->data<=next1->data)){
            //node addition
            curr1 -> next = curr2;
            next2 = curr2->next;
            curr2-> next = next1;
            
            //pointer updation
            curr1 = curr2;
            curr2 = next2;
        }else{
            //curr1 and next1 ko aage badhana padega //one step ahead in first list
            curr1 = curr1->next;//curr1=next1;
            next1 = next1->next;

            if(next1==NULL){//agar list khatam ho gyi then greater elements end mein chipka do
                curr1 ->next = curr2;
                return first;
            }
        }
    }
    return first;

}



Node<int>* sortTwoLists(Node<int>* first, Node<int>* second){
    if(first==NULL){
        return second;
    }
    if(second==NULL)
        return first;

    if(first->data <= second->data){
        return solve(first,second);
    }else{
        return solve(second,first);
    }

}

int main() {
    
    return 0;
}
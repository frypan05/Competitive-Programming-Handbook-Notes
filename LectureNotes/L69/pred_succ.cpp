#include<bits/stdc++.h>
using namespace std;

class node{
    public:
    int data;
    node* left;
    node* right;

    node( int d){
        data = d; 
        left = right = NULL;
    }
};


node* inOrderPredecessor(node* root , int d){
    node* predecessor = NULL;
    node* temp = root;
    while(temp!=NULL){
        
        if(temp->data > d){
            temp = temp->left;
        }else if(temp->data < d){
            predecessor = temp; //potential ans
            temp = temp->right;
        }else{ //node with d
            if(temp->left!=NULL){
                // Case 1: Node has a left subtree. //Predecessor is the rightmost node in the left subtree.
                node* curr =  temp->left;
                while(curr->right!=NULL){
                    curr = curr->right;
                }
                return curr;
            }else{// Case 2: Node has no left subtree.// Predecessor is the closest ancestor for which this node is in its right subtree.
                return predecessor;
            }
        }
    }
    return NULL;
}
node* inOrderSuccessor(node* root , int d){
    node* successor = NULL;
    node* temp = root;
    while(temp!=NULL){
        
        if(temp->data > d){
            successor = temp; //potential successor
            temp = temp->left;
        }else if(temp->data < d){
            temp = temp->right;
        }else{//node with d   
            if(temp->right!=NULL){
                // Case 1: Node has a right subtree. //Predecessor is the leftmost node in the right subtree.
                node* curr =  temp->right;
                while(curr->left!=NULL){
                    curr = curr->left;
                }
                return curr;
            }else{// Case 2: Node has no right subtree.// Successor is the closest ancestor for which this node is in its left subtree.
                return successor;
            }
        }
    }
    return NULL;
}

int main() {
    
    node* root =NULL;

        
    //10 8 21 7 27 5 4 3 -1
    cout<<"inorder predecessor"<<inOrderPredecessor(root,21)->data<<endl;
    cout<<"inorder successor"<<inOrderSuccessor(root,21)->data<<endl;


    return 0;
}
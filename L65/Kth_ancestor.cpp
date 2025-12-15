//https://www.geeksforgeeks.org/problems/kth-ancestor-in-a-tree/1

#include<bits/stdc++.h>
using namespace std;

//approach 1 : root to node -> find path ,store in vector


//aproach 2 : recursion
Node* solve(Node* root,int & k, int node){
    //base case
    if(root==NULL) return NULL;
    if(root->data == node) return root;

    Node* leftAns = solve(root->left,k,node);
    Node* rightAns = solve(root->right,k,node);

    if(leftAns != NULL && rightAns == NULL){
        k--;
        if(k<=0){ //to handle faulty value of k
            //answer lock
            k=INT_MAX;
            return root;
        }
        return leftAns;
    }
    if(leftAns == NULL && rightAns != NULL){
        k--;
        if(k<=0){ //to handle faulty value of k
            //answer lock
            k=INT_MAX;
            return root;
        }
        return rightAns;
    }
    return NULL;
}

int kthAncestor(node* root, int k , int node){
    Node* ans = solve(root,k,node);
    if(ans==NULL || ans->data==node) return -1;
    else return ans->data;
}

int main() {
    
    return 0;
}
//https://www.geeksforgeeks.org/problems/lowest-common-ancestor-in-a-binary-tree/1

#include<bits/stdc++.h>
using namespace std;

Node* lca(Node* root,int n1, int n2){
    //base case
    if(root==NULL) return NULL;
    if(root->data==n1 || root->data==n2) return root;

    Node* leftAns = lca(root->left,n1,n2);
    Node* rightAns = lca(root->right, n1 , n2);

    if(leftAns!=NULL && rightAns!=NULL) return root;
    else if(leftAns!=NULL && rightAns==NULL) return leftAns;
    else if(leftAns==NULL && rightAns!=NULL) return rightAns;
    else return NULL;

}     

int main() {
    
    return 0;
}
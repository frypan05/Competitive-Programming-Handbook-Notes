//https://www.geeksforgeeks.org/problems/boundary-traversal-of-binary-tree/1

#include<bits/stdc++.h>
using namespace std;

void traverseLeft(Node* root,vector<int>& ans){
    if(root==nullptr || (root->left!=NULL && root->right!=NULL)){
        return;
    }
    
    ans.push_back(root->data);
    if(root->left) {traverseLeft(root->left,ans);}
    else{ traverseLeft(root->right,ans);}
    
}

void traverseLeaf(Node*root, vector<int> & ans){
    if(root==nullptr){
        return;
    }
    if((root->left!=NULL && root->right!=NULL)){
        ans.push_back(root->data);
        return;
    }

    traverseLeft(root->left,ans);
    traverseLeft(root->right,ans);
} 

void traverseRight(Node* root,vector<int>& ans){
    if(root==NULL || (root->left!=NULL && root->right!=NULL)){
        return;
    }
    
    
    if(root->right) traverseRight(root->right,ans);
    else traverseRight(root->left,ans);

    ans.push_back(root->data); //for reverse order
    
}

vector<int> boundaryTraversal(Node* root){
    vector<int> ans;
    if(root==NULL) return ans;
    
    ans.push_back(root->data);

    //left part traverse
    traverseLeft(root,ans);

    //traverse leaf node
    
    //left subtree
    traverseLeaf(root->left,ans);
    //right subtree
    traverseLeaf(root->right,ans);

    //traverse right part
    traverseRight(root->right,ans);

    return ans;
}

int main() {
    
    return 0;
}

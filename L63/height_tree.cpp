//https://www.geeksforgeeks.org/problems/height-of-binary-tree/1

#include<bits/stdc++.h>
using namespace std;


class Node {
public:
    int data;
    Node* left;
    Node* right;

    Node(int val) {
        data = val;
        left = right = NULL;
    }
};


int height(Node* root) {
    //base case
    if(root==NULL){
        return 0;
    }
    if(!root->left && !root->right)return 0;
    
    int left = height(root->left);
    int right = height(root->right);
    int ans = max(left,right)+1;

    return ans;
}
        
int height(Node* root) {
    //base case
    if(root==NULL){
        return -1;
    }
    
    
    int left = height(root->left);
    int right = height(root->right);
    int ans = max(left,right)+1;

    return ans;
}


int main() {
    
    return 0;
}
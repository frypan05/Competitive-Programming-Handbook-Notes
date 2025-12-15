//https://www.geeksforgeeks.org/problems/sum-tree/1

#include<bits/stdc++.h>
using namespace std;


pair<bool,int> isSumTreeFast(Node* root) {
    if(root==NULL){
        pair<bool,int> p = make_pair(true,0);
        return p;
    }
    if(root->left==NULL && root->right ==NULL){
        pair<bool,int> p = make_pair(true,root->data);
        return p;
    }

    pair<bool,int> leftAns = isSumTreeFast(root->left);
    pair<bool,int> rightAns = isSumTreeFast(root->right);

    bool left = leftAns.first;
    bool right = rightAns.first;

    int leftSum = leftAns.second;
    int rightSum = rightAns.second;

    bool condn = root->data == leftSum+rightSum;

    pair<bool ,int> ans;

    if(left && right && condn){
        ans.first = true;
        ans.second = 2*root->data; //since left subtree and right subtree have same sum // we send twice the value above
    }else{  
        ans.first = false;
    }
    return ans;
}

bool isSumTree(Node* root){
    return isSumTreeFast(root).first;
}

int main() {
    
    return 0;
}
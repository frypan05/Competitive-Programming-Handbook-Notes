//https://www.geeksforgeeks.org/problems/left-view-of-binary-tree/1

#include<bits/stdc++.h>
using namespace std;

//approach 1 : level order


//approach 2 : recursion
void solve(Node* root,vector<int>& ans,int level){
    if(root==NULL) return;

    //we entered into a new level
    if(level==ans.size()) ans.push_back(root->data); 

    solve(root->left,ans,level+1);
    solve(root->right,ans,level+1);

    return;
}

vector<int> leftView(Node* root){
    vector<int> ans;
    solve(root,ans,0);
    return ans;
}

int main() {
    
    return 0;
}
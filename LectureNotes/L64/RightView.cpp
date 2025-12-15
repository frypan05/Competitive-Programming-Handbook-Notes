//https://www.geeksforgeeks.org/problems/right-view-of-binary-tree/1

#include<bits/stdc++.h>
using namespace std;

//approach 1 : level order


//approach 2 : recursion
//leftView And rightView only defer in the order of recursive call for traversal direction 

void solve(Node* root,vector<int>& ans,int level){
    if(root==NULL) return;

    //we entered into a new level
    if(level==ans.size()) ans.push_back(root->data); 

    solve(root->right,ans,level+1);
    solve(root->left,ans,level+1);
     
    return;
}

vector<int> rightView(Node* root){
    vector<int> ans;
    solve(root,ans,0);
    return ans;
}

int main() {
    
    return 0;
}
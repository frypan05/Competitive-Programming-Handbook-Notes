//https://www.geeksforgeeks.org/problems/k-sum-paths/1

#include<bits/stdc++.h>
using namespace std;

void solve(Node* root,int k , int &count,vector<int>path){
    //base case
    if(root==NULL) return;

    path.push_back(root->data); 

    solve(root->left,k,count,path);
    solve(root->right,k,count,path);

    //check for k sum
    int size = path.size();
    int sum=0;
    for (int i =size-1;i>=0;i--){
        sum+=path[i];
        if(sum==k) count++;
    }

    //path.pop_back(); //unnecessary as we arent passing path by reference
}

int sumK(Node* root,int k){
    vector<int> path;
    int count =0;
    solve(root,k,count,path);
    return count;
}

int main() {
    
    return 0;
}
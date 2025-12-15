//https://www.geeksforgeeks.org/problems/construct-tree-1/1

#include<bits/stdc++.h>
using namespace std;


//approach 1 //O(n2)
int findPosition(vector<int> &inorder, int element,int n){
    for(int i=0;i<n;i++){
        if(inorder[i] == element) return 1;
    }
    return -1;
}

Node* solve(vector<int> &inorder, vector<int> &preorder ,int & index, int inOrderStart, int inOrderEnd ,int n){
    //base case
    if(index >= n || inOrderStart > inOrderEnd) return NULL;

    int element = pre[index++];
    Node* root = new Node(element);
    int position = findPosition(inorder,element,n);

    root->left = solve(inorder,preorder, index ,inOrderStart,position-1,n);
    root->right = solve(inorder,preorder, index ,position+1,inOrderEnd,n);

    return root;
}

Node *buildTree(vector<int> &inorder, vector<int> &preorder) {
    int n = inorder.size();
    int preOrderIndex =0;
    Node* ans= solve(inorder, preorder , preOrderIndex,0,n-1,n);
    return ans;
}

//approach 2 : optimisation: used map //O(nlogn)

void createMapping(vector<int> &inorder, map<int,int>& nodeToIndex, int element,int n){
    for(int i=0;i<n;i++){
        nodeToIndex[inorder[i]] = i;
    }
}

Node* solve(vector<int> &inorder, vector<int> &preorder ,int & index, int inOrderStart, int inOrderEnd, map<int,int>& nodeToIndex ,int n){
    //base case
    if(index >= n || inOrderStart > inOrderEnd) return NULL;

    //create a root node for element
    int element = pre[index++];
    Node* root = new Node(element);

    //find element's index in inorder
    int position = nodeToIndex[element];

    root->left = solve(inorder,preorder, index ,inOrderStart,position-1,n,nodeToIndex);
    root->right = solve(inorder,preorder, index ,position+1,inOrderEnd,n,nodeToIndex);

    return root;
}

Node *buildTree(vector<int> &inorder, vector<int> &preorder) {
    int n = inorder.size();
    int preOrderIndex =0;
    map<int,int> nodeToIndex;
    createMapping(inorder,nodeToIndex,n);

    Node* ans= solve(inorder, preorder , preOrderIndex,0,n-1,n,nodeToIndex);
    return ans;
} 


int main() {
    
    return 0;
}
//https://www.geeksforgeeks.org/problems/tree-from-postorder-and-inorder/1

#include<bits/stdc++.h>
using namespace std;


void createMapping(vector<int> &inorder, map<int,int>& nodeToIndex, int element,int n){ //O(nlogn)
    for(int i=0;i<n;i++){
        nodeToIndex[inorder[i]] = i;
    } 
}

Node* solve(vector<int> &inorder, vector<int> &postorder ,int & index, int inOrderStart, int inOrderEnd, map<int,int>& nodeToIndex ,int n){ //O(n)
    //base case
    if(index < 0 || inOrderStart > inOrderEnd) return NULL;

    //create a root node for element
    int element = post[index--];
    Node* root = new Node(element);

    //find element's index in inorder
    int position = nodeToIndex[element];

    root->right = solve(inorder,postorder, index ,position+1,inOrderEnd,n,nodeToIndex);
    root->left = solve(inorder,postorder, index ,inOrderStart,position-1,n,nodeToIndex);

    return root;
}

Node *buildTree(vector<int> &inorder, vector<int> &postorder) {
    int n = inorder.size();
    int postOrderIndex =n-1;
    map<int,int> nodeToIndex;
    createMapping(inorder,nodeToIndex,n);

    Node* ans= solve(inorder, postorder , postOrderIndex,0,n-1,n,nodeToIndex);
    return ans;  
}

  
int main() {
    
    return 0;
}
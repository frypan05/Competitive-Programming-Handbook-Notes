#include<bits/stdc++.h>
using namespace std;

template <typename T>
class BinaryTreeNode {
  public :
    T data;
    BinaryTreeNode<T> *left;
    BinaryTreeNode<T> *right;

    BinaryTreeNode(T data) {
            this -> data = data;
            left = NULL;
            right = NULL;
    }
};

void inOrder(BinaryTreeNode* root, int &count){
    if(root==NULL){
        return;
    }

    inOrder(root->left,count);
    if(root->left==NULL && root->right==NULL) count++; //leaf node condn
    inOrder(root->right,count);
}

//nodes with left and right node null || nodes with no child
//using inorder to traverse
int noOfLeafNodes(BinaryTreeNode<int> *root){ 
    int cnt =0;
    inOrder(root,cnt);
    return cnt;
}

int main() {
    
    return 0;
}
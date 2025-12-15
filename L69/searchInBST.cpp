#include<bits/stdc++.h>
using namespace std;

template <typename T>
class BinaryTreeNode{
    public:
        T data;
        BinaryTreeNode<T> *left, *right;
        BinaryTreeNode() : data(0), left(NULL), right(NULL) {}
        BinaryTreeNode(T x) : data(x), left(NULL), right(NULL) {}
        BinaryTreeNode(T x, BinaryTreeNode<T> *left, BinaryTreeNode<T> *right) : data(x), left(left), right(right) {}
};

bool searchInBST(BinaryTreeNode<int> *root, int x) { //recursively
    if(root == NULL) return false;
    if(root->data == x) return true;

    if(x > root->data) root = searchInBST(root->right,x) ;
    if(x < root->data) root = searchInBST(root->left,x);
}

bool searchInBST(BinaryTreeNode<int> *root, int x) { //iteratively
    BinaryTreeNode<int> *temp = root;

    while(temp!=NULL){
        if(temp->data == x){
            return true;
        }
        if(temp->data>x){
            temp = temp->left;
        }else{
            temp = temp->right;
        }
    }
    return false;
} 

int main() {
    
    return 0;
}
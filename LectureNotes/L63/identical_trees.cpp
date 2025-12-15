//https://www.geeksforgeeks.org/problems/determine-if-two-trees-are-identical/1

#include<bits/stdc++.h>
using namespace std;

class Node {
public:
    int data;
    Node* left;
    Node* right;

    Node(int x) {
        data = x;
        left = nullptr;
        right = nullptr;
    }
};

bool isIdentical ( Node * r1, Node * r2){
    //base case
    if(r1==NULL && r2==NULL){
        return true;
    }
    if(r1==NULL && r2!=NULL){
        return false;
    }
    if(r1!=NULL && r2==NULL){
        return false;
    }
    
    bool left = isIdentical(r1->left,r2->left);
    bool right = isIdentical(r1->right,r2->right);

    bool value = r1->data == r2->data;

    if(left && right &&value){
        return true;
    }else{
        return false;
    }
}

int main() {
    
    return 0;
}
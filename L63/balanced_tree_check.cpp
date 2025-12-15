//https://www.geeksforgeeks.org/problems/check-for-balanced-tree/1

#include<bits/stdc++.h>
using namespace std;

class Node {
  public:
    int data;
    Node* left;
    Node* right;

    // Constructor to initialize a new node
    Node(int val) {
        data = val;
        left = NULL;
        right = NULL;
    }
};

//approach 1  -  O(n2)

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

bool isBalanced(Node* root) {
    //base case
    if(root==NULL){
        return true;
    }

    bool left = isBalanced(root->left);
    bool right = isBalanced(root->right);

    bool diff = abs (height(root->left) - height(root->right)) <=1;

    if(left && right && diff){
        return true;
    }else{
        return false;
    }

}

//approach 2 - pair - o(n)


pair<bool,int> isBalancedFast(Node* root) {
    //base case
    if(root==NULL){
        pair<bool,int> p = make_pair(true,0);
        return p;
    } 

    //first value in pair is isBalanced and second value is height
    pair<bool,int> left = isBalancedFast(root->left);
    pair<bool,int> right = isBalancedFast(root->right);
    
    bool leftAns = left.first;
    bool rightAns = right.first;
    bool diff = abs (left.second - right.second) <=1;

    pair<bool,int> ans;
    ans.second = max(left.second,right.second) +1;

    if(leftAns && rightAns && right){
        ans.first = true;
    }else{
        ans.first = false;
    }

    return ans;
}

bool isBalanced(Node* root){
    return isBalancedFast(root).first;
}

int main() {
    
    return 0;
}
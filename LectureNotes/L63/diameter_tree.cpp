//https://www.geeksforgeeks.org/problems/diameter-of-binary-tree/1

#include<bits/stdc++.h>
using namespace std;

class Node {
public:
    int data;
    Node* left;
    Node* right;

    Node(int val) {
        data = val;
        left = NULL;
        right = NULL;
    }
};

//aproach 1 : TLE on Skewed - O(n2)

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

int diameter(Node* root) {
    //base case
    if(root==NULL){
        return -1;
    }
    
    
    int op1 = diameter(root->left);
    int op2 = diameter(root->right);
    int op3 = height(root->left)+height(root->right);

    int ans = max(op1,max(op2,op3));
    return ans;
        
}

//approach 2
// It returns the height of the subtree rooted at 'root'.
// It updates the maximum diameter found so far in 'max_diameter'.
int height_and_diameter(Node* root, int& max_diameter) {
    // base case
    if (root == NULL) {
        return 0;
    }
    
    // 1. Recursively get height for left and right subtrees.
    int left_height = height_and_diameter(root->left, max_diameter);
    int right_height = height_and_diameter(root->right, max_diameter);
    
    // 2. Calculate diameter passing through the current root.
    int current_diameter = left_height + right_height;
    
    // 3. Update the global maximum diameter found so far.
    max_diameter = max(max_diameter, current_diameter);

    // 4. Return the height of the current subtree.
    return max(left_height, right_height) + 1;
}


int diameter(Node* root) {
    
    if (root == NULL) {
        return 0;
    }
    
    int max_diameter = 0;
    height_and_diameter(root, max_diameter);
    return max_diameter;
}

//approach 3: using pairs - O(n)

pair<int,int> diamterFast(Node* root) {
    //base case
    if(root==NULL){
        pair<int,int> p = make_pair(0,0);
        return p;
    }

    //first value in pair is diameter and second value is height
    pair<int,int> left = diamterFast(root->left);
    pair<int,int> right = diamterFast(root->right);
    
    int op1 = left.first;
    int op2 = right.first;
    int op3 = left.second + right.second + 1;

    pair<int,int> ans;
    ans.first = max(op1,max(op2,op3));
    ans.second = max(left.second,right.second) +1;

    return ans;
}

int diameter(Node* root) {
    
    return diamterFast(root).first;
}




int main() {
    
    return 0;
}
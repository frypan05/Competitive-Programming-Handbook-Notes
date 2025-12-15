//https://www.geeksforgeeks.org/problems/zigzag-tree-traversal/1

#include<bits/stdc++.h>
using namespace std;

vector<int> zigZagTraversal(Node* root) {
    vector<int> result;
    if(root==NULL){
        return result;
    }

    queue<Node*> q;
    q.push(root);

    //flag
    bool leftToRight = true;

    while(!q.empty()){
        int size = q.size();
        vector<int> ans(size); //level based vector
        //level process
        for(int i =0;i<size;i++){
            Node* frontNode = q.front();
            q.pop();

            //normal or reverse insert
            int index = leftToRight ? i:size-1;
            ans[index] = frontNode ->data;

            //level order traversal
            if(frontNode->left){
                q.push(frontNode->left);
            }
            if(frontNode->right){
                q.push(frontNode->right);
            }
        }
        //direction change 
        leftToRight = !leftToRight;

        for(auto i:ans){
            result.push_back(i);
        }
    }
    return result;
}

int main() {
    
    return 0;
}
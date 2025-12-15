#include<bits/stdc++.h>
using namespace std;

class node{
    public:
        int data;
        node* left;
        node* right;

    node(int d){
        this->data=d;
        this->left=NULL;
        this->right=NULL;   
    }

};

node* buildTree(node* root){
    //data
    cout<<" enter the data: "<<endl;
    int data;
    cin>>data;
    root = new node(data);

    if( data == -1){
        return NULL;
    }

    //left and right node
    cout<<"enter data for inserting in left of "<<data<<endl;
    root->left = buildTree(root->left);

    cout<<"enter data for inserting in right of "<<data<<endl;
    root->right = buildTree(root->right);

    return root;  
}

void levelOrderTraversal(node* root){ //bfs
    queue<node*> q;
    q.push(root);
    q.push(NULL); //separator for tree pattern like o/p

    while(!q.empty()){
        node* temp = q.front();
        q.pop();

        if(temp==NULL){ 
            //purana level complete traverse ho chuka hai
            cout<<endl;
            if(!q.empty()){//queue still has some child nodes
                q.push(NULL);
            }
        }else{
            cout<<temp->data<<" ";
            if(temp->left){
                q.push(temp->left);
            }
            if(temp->right){
                q.push(temp->right);
            }
        }
    }
}  
void buildFromLevelOrder(node* &root){ //creating tree with level order
    queue<node*> q;
    cout<<" enter data for root"<<endl;
    int data;
    cin>>data;    
    root = new node(data);
    q.push(root);

    while(!q.empty()){
        node* temp = q.front();
        q.pop();

        cout<<"enter left node for "<<temp->data<<endl;
        int leftData;
        cin>>leftData;

        if(leftData!=-1){
            temp->left = new node(leftData);
            q.push(temp->left);
        }

        cout<<"enter right node for "<<root->data<<endl;
        int rightData;
        cin>>rightData;

        if(rightData!=-1){
            temp->right = new node(rightData);
            q.push(temp->right);
        }
    }
}
void reverseLevelOrderTraversal(node* root){ //bfs
    queue<node*> q;
    stack<node*> st;
    q.push(root);  

    while(!q.empty()){
        node* temp = q.front();
        q.pop();
        st.push(temp);
        
        //order of pushing left and right matters
        if(temp->right){
            q.push(temp->right);
        } 
        if(temp->left){
            q.push(temp->left);
        }
        
    }

    while(!st.empty()){
        node* temp = st.top();
        cout<<temp->data<<" ";
        st.pop();
    }
} 

//DFS - recursive methods
void inOrder(node* root){ //LNR
    //base case
    if(root==NULL){
        return;
    }

    //L 
    inOrder(root->left);
    //N
    cout<<root->data<<" ";
    //R
    inOrder(root->right);
}
void preOrder(node* root){ //NLR
        //base case
    if(root==NULL){
        return;
    }

    
    //N
    cout<<root->data<<" ";
    //L 
    preOrder(root->left);
    //R
    preOrder(root->right);
}
void postOrder(node* root){ //LRN
        //base case
    if(root==NULL){
        return;
    }

    //L 
    postOrder(root->left);
    //R
    postOrder(root->right);
    //N
    cout<<root->data<<" ";
}

int main() {

    node* root =NULL;

    //creating a tree
    //root = buildTree(root); //tree building with recursion
    //1 3 7 -1 -1 11 -1 -1 5 17 -1 -1 -1

    buildFromLevelOrder(root); ////tree building with level order
    levelOrderTraversal(root);
    //1 3 5 7 11 17 -1 -1 -1 -1 -1 -1 -1


    //level order
    cout<<"level order traversal"<<endl;
    levelOrderTraversal(root);
    cout<<"\n\n";

    cout<<"reverse level order"<<endl;
    reverseLevelOrderTraversal(root);
    cout<<"\n\n";

    cout<<"inorder traversal"<<endl;
    inOrder(root);
    cout<<"\n\n";

    cout<<"preorder traversal"<<endl;
    preOrder(root);
    cout<<"\n\n";

    cout<<"post order traversal"<<endl;
    postOrder(root);

    return 0;
}
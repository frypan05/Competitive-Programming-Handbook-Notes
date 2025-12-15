#include<bits/stdc++.h>
using namespace std;

class node{
    public:
    int data;
    node* left;
    node* right;

    node(int d){
        this->data=d;
        right = left = NULL;
    }
};

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

node* insertIntoBST(node* root , int d){
    //base case
    if(root==NULL){
        root = new node(d);
        return root;
    }

    if(d > root->data) root->right = insertIntoBST(root->right,d); //right part me insert karna hai
    if(d < root->data) root->left = insertIntoBST(root->left,d); //left part mein insert karna hai

    return root;
}

void takeInput(node* &root){
    int data;
    cin>>data;

    while(data!=-1){
        root = insertIntoBST(root,data);
        cin>>data;
    }
}

node* minValue(node* root){
    node* temp = root;
    while(temp->left!=NULL){
        temp=temp->left;
    }
    return temp;
}
node* maxValue(node* root){
    node* temp = root;
    while(temp->right!=NULL){
        temp=temp->right;
    }
    return temp;
}

node* deleteFromBST(node* root , int d){
    //base case
    if(root==NULL){
        return root;
    }
    if(root->data ==d){
        //0 child
        if(root->left==NULL && root->right ==NULL){
            delete root;
            return NULL;
        }


        //1 child
        //left child
        if(root->left!=NULL && root->right==NULL){
            node* temp = root->left;
            delete root;
            return temp;
        }
        //right child
        if(root->left==NULL && root->right!=NULL){
            node* temp = root->right;//pointing to right child
            delete root;//deleting root
            return temp;//returning right child to parent node
        }


        //2 child
        if(root->left!=NULL && root->right!=NULL){
            int mini = minValue(root->right)->data; //finding min value in right subtree
            root->data = mini;//and copying it to root
            root->right = deleteFromBST(root->right,mini);//deleting the duplicate node from right subtree
            return root;
        }

    }else if(root->data > d){
        //left part mein jao
        root->left = deleteFromBST(root ->left , d);
        return root;
    }else{
        //right part mein jao
        root->right = deleteFromBST(root ->right , d);
        return root;
    }

}


int main() {

    node* root = NULL;

    //10 8 21 7 27 5 4 3 -1
    cout<<"enter data to create bst"<<endl;
    takeInput(root);

    cout<<"printing the bst"<<endl<<"level order traversal"<<endl;
    levelOrderTraversal(root);

    cout<<"inorder traversal"<<endl;
    inOrder(root);
    cout<<"\n\n";

    cout<<"preorder traversal"<<endl;
    preOrder(root);
    cout<<"\n\n";

    cout<<"post order traversal"<<endl;
    postOrder(root);
    cout<<"\n\n";

    cout<<"min value is "<<minValue(root)->data<<endl;
    cout<<"max value is "<<maxValue(root)->data<<endl<<endl;

    //50 20 10 30 70 90 110 -1

    root = deleteFromBST(root,30);

    cout<<"printing the bst"<<endl;
    levelOrderTraversal(root);

    cout<<"inorder traversal"<<endl;
    inOrder(root);
    cout<<"\n\n";

    cout<<"preorder traversal"<<endl;
    preOrder(root);
    cout<<"\n\n";

    cout<<"post order traversal"<<endl;
    postOrder(root);
    cout<<"\n\n";

    cout<<"min value is "<<minValue(root)->data<<endl;
    cout<<"max value is "<<maxValue(root)->data<<endl<<endl;

    return 0;
}

#include<bits/stdc++.h>
using namespace std;

class node{
    public:
    int data;
    node* left;
    node* right;

    node(int d){
        this->data = d;
        left = right = NULL;
    }
};

// class node{
//     int data;
//     node*left;
//     node* right;
//     node(int d):data(d),left(NULL),right(NULL){}
// };

node* buildTree(node* root){
    //data
    cout<<"enter the data: "<<endl;
    int data;
    cin>>data;
    root = new node(data);
    if(data == -1){
        return NULL;
    }

    //left and right node
    cout<<"enter data for inserting in left of"<<data<<endl;
    root->left = buildTree(root->left);

    cout<<"enter data for inserting in right of"<<data<<endl;
    root->right = buildTree(root->right);

    return root;

}

//DFS - iterative methods

//1. Inorder

// void inOrder(node* root){
//     if(root==NULL){
//         return;
//     }
//     stack<node*> st;
//     node * temp =root;
//     while(temp!=NULL || !st.empty()){
//         //reach left most node - L
//         while(temp!=NULL){
//             st.push(temp);
//             temp = temp->left;
//         }
//         //print node and pop - N
//         temp = st.top();
//         st.pop();
//         cout<<temp->data<<" ";
//         //reach right most node - R
//         temp = temp->right;
//     }
// }

void inOrder(node* root, vector<int> & res){
    if (root==NULL) return;
    inOrder(root->left,res);
    res.push_back(root->data);
    inOrder(root->right,res);
    
}

vector<int> inOrderTravesal(node* root){
    stack<node*> st;
    node* node = root;
    vector<int> inorder;
    while(true){
        if(node!=NULL){
            st.push(node);
            node = node->left;
        }else{

            if(st.empty()) break;
            node = st.top();
            st.pop();
            inorder.push_back(node->data);
            node = node->right;
        }
    }
    return inorder;
}

//2. preorder

/* go to cpp _ PREORDER

func dfs(root *Node) []*Node{
    visited := []*Node{}
    stack := []*Node{root}
    
    if root == nil{
        return visited
    }
    
    for len(stack) != 0{
        node := stack[len(stack)-1]
        visited = append(visited, node)
        stack = stack[:len(stack)-1] //lifo
        
        if node.Right != nil{
            stack = append(stack, node.Right)
        }
        
        if node.Left != nil{
            stack = append(stack, node.Left)
        }
    }
    return visited
}
    
std::vector<Node*> dfs(Node* root) {
    std::vector<Node*> visited;
    if (root == nullptr) {
        return visited;
    }

    std::stack<Node*> st;
    st.push(root);

    while (!st.empty()) {
        Node* node = st.top();
        st.pop();
        
        visited.push_back(node);

        // Push the right child first, so the left child is processed next (LIFO)
        if (node->Right != nullptr) {
            st.push(node->Right);
        }
        
        if (node->Left != nullptr) {
            st.push(node->Left);
        }
    }
    return visited;
}


*/

void preOrder(node* root, vector<int> & res){
    if (root==NULL) return;
    res.push_back(root->data);
    preOrder(root->left,res);
    preOrder(root->right,res);
    
}


void preOrder(node* root){

// Append root node to the back of the stack.
// Remove the last element from the stack and visit it.
// If this element has a right child, we append the right child to the stack.
// If this element has a left child, we append the left child to the stack.
// Repeat steps 2–4 until the stack is empty.

    if(root==NULL){
        return;
    }
    stack<node*> st;
    node * temp =root;
    st.push(root);

    while(!st.empty()){
        //print node and pop 
        temp = st.top();
        st.pop();
        cout<<temp->data<<" ";

        //push right child 
        if(temp->right) st.push(temp->right);
            
        //push left child
        if(temp->left) st.push(temp->left);
    }
}
vector<int> preOrderTraversal(node* root){
    vector<int> preorder;
    if(root == NULL) return preorder;

    stack<node*> st;
    st.push(root);
    while(!st.empty()){
        root = st.top();
        st.pop();
        preorder.push_back(root->data);
        if(root->right) st.push(root->right);
        if(root->left) st.push(root->left);
    }
    return preorder;
}

//3. postorder

void postOrder(node* root, vector<int> & res){
    if (root==NULL) return;
    postOrder(root->left,res);
    postOrder(root->right,res);
    res.push_back(root->data);
}

vector<int> postOrderTraversal(node* root){

}


int main() {
    node* root =NULL;
    root = buildTree(root);

    //1 3 7 -1 -1 11 -1 -1 5 17 -1 -1 -1
    
    cout<<"inorder traversal"<<endl;
    //inOrder(root);
    vector<int> inorder = inOrderTravesal(root);
    for(auto i:inorder){ // in each iteration, the variable i is assigned the value of the element at the current position, not the index.
        cout<<i<<" ";
    }
    cout<<"\n\n";

    cout<<"preorder traversal"<<endl;
    //preOrder(root);
    vector<int> preorder = preOrderTraversal(root);
    for( auto i: preorder){
        cout<<i<<" ";
    }
    cout<<"\n\n";

    cout<<"post order traversal"<<endl;
    postOrder(root);

    return 0;
}
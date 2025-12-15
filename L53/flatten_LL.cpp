//https://www.naukri.com/code360/problems/flatten-a-linked-list_1112655

#include<bits/stdc++.h>
using namespace std;

class Node {
    public:
		int data;
		Node *next;
 		Node *child;
		Node() : data(0), next(nullptr), child(nullptr){};
		Node(int x) : data(x), next(nullptr), child(nullptr) {}
		Node(int x, Node *next, Node *child) : data(x), next(next), child(child) {}
};

Node* flattenLinkedList(Node* head) 
{
	
}

int main() {
    
    return 0;
}
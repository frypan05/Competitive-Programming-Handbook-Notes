#include <iostream>
using namespace std;

// Node structure
class Node {
public:
    int data;
    Node* next;
    
    Node(int x) {
        data = x;
        next = NULL;
    }
};

// Stack implementation using linked list
class Stack {
    Node* top;
    int count;
    
public:
    Stack() {
        
        // initially stack is empty
        top = NULL;
        count = 0;
    }

    // push operation
    void push(int x) {
        Node* temp = new Node(x);
        temp->next = top;
        top = temp;
        
        count++;
    }

    // pop operation
    int pop() {
        if (top == NULL) {
            cout << "Stack Underflow" << endl;
            return -1;
        }
        Node* temp = top;
        top = top->next;
        int val = temp->data;
        
        count--;
        delete temp;
        return val;
    }

    // peek operation
    int peek() {
        if (top == NULL) {
            cout << "Stack is Empty" << endl;
            return -1;
        }
        return top->data;
    }

    // check if stack is empty
    bool isEmpty() {
        return top == NULL;
    }

    // size of stack
    int size() {
        return count;
    }
};

int main() {
    Stack st;

    // pushing elements
    st.push(1);
    st.push(2);
    st.push(3);
    st.push(4);

    // popping one element
    cout << "Popped: " << st.pop() << endl;

    // checking top element
    cout << "Top element: " << st.peek() << endl;

    // checking if stack is empty
    cout << "Is stack empty: " << (st.isEmpty() ? "Yes" : "No") << endl;

    // checking current size
    cout << "Current size: " << st.size() << endl;

    return 0;
}
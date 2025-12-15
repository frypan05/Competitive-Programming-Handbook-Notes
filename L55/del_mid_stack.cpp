#include<bits/stdc++.h>
using namespace std;

void solve(stack<int>& inputStack , int count , int size){
    //base case
    if(count == size/2){
        inputStack.pop();
        return;    
    }    
    
    int num = inputStack.top();
    inputStack.pop();

    //recursive call
    solve(inputStack , count+1, size);

    inputStack.push(num);
}

void deleteMiddle(stack<int>& inputStack , int n){
    int count = 0;
    solve(inputStack,count,n);
    
}


int main() {
    
    return 0;
}
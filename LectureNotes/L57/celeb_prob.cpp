#include<bits/stdc++.h>
using namespace std;

//https://www.geeksforgeeks.org/problems/the-celebrity-problem/1

bool knows (vector<vector<int> >& mat,int a,int b,int n){
    return mat[a][b] == 1;    
}

int celebrity(vector<vector<int> >& mat) {

    int n = mat.size();
    if(n==0){ //empty matrix case
        return -1;
    }
    if(n==1){ //single person is always a celebrity
        return 0;
    }
    stack<int> s;
    
    //step1: push all element in stack
    for(int i =0;i<n;i++){
        s.push(i);
    }

    //step2 : get two elements and compare them
    while(s.size()>1){
        int a = s.top();
        s.pop();
        int b = s.top();
        s.pop();

        if(knows(mat,a,b,n)){
            s.push(b);
        }else{
            s.push(a);
        }
    }

    int ans = s.top();

    //step3 : single element in stack is potential celebrity // verify
    //row check
    int zeroCount = 0;

    for(int i = 0;i<n;i++){
        if(mat[ans][i] == 0){
            zeroCount++;
        }
    }
    if(zeroCount!=n-1){//n-1 zeroes as except himself , celebrity knows nobody
        return -1;
    }

    //column check
    int oneCount = 0;

    for(int i = 0;i<n;i++){
        if(mat[i][ans] == 1){
            oneCount++;
        }
    }
    if(oneCount != n){ //n as celebrity will also know himself
        return -1;
    }

    return ans;
        
}


//2 pointer approach


int main() {
    
    return 0;
}
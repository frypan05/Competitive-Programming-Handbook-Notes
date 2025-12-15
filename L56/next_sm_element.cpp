#include<bits/stdc++.h>
using namespace std;

//https://www.naukri.com/code360/problems/next-greater-element_1112581

vector<int> nextSmallerElement(vector<int> &arr, int n)
{
    stack<int> s;
    s.push(-1);
    vector<int> ans(n);

    for(int i =n-1;i>=0;i--){
        int curr = arr[i];
        while(s.top()>=curr){ // ans is not stack ka top
            s.pop();
        }
        //ans is stack ka top
        ans[i]=s.top();
        s.push(curr);
    }
    return ans;
}

int main() {
    
    return 0;
}
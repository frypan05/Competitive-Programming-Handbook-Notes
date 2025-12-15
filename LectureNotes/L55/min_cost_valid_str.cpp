#include<bits/stdc++.h>
using namespace std;

int findMinimumCost(string str) {
    
    //odd condition
    if(str.lenth()%2==1){
        return -1;
    }

    stack<char> s;
    for (int i=0;i<str.length();i++){
        char ch = str[i];
        if(ch=='{'){
            s.push(ch);
        }else{
            //filter out valid parts from str
            //when ch==')'
            if(!s.empty() && s.top()=='{'){
                s.pop();
            }else{
                s.push(ch);
            }
        }

    }
    //stack contains invalid expression only
    int a =0,b=0;
    while(!s.empty()){
        if(s.top()=='{'){
            b++;
        }else{
            a++;
        }
        s.pop();
    }
    int ans = (a+1)/2 + (b+1)/2;
    return ans;
}
 
int main() {
    
    return 0;
}
#include<bits/stdc++.h>
using namespace std;

int main() {

    string str = "nine";
    stack<char> s;

    for(int i =0;i<str.length();i++){
        char ch = str[i];
        s.push(ch);
    }
    
    string ans="";
    for(int i =0;i<str.length();i++){
        ans.push_back(s.top());    //ans.append(1,s.top()) 
        s.pop();
    }

    cout<<ans;

    return 0;
}
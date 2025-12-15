#include<bits/stdc++.h>
using namespace std;

bool hasSameDigits(string s) {
    int start = 0;
    int end = s.size() - 1; 
    
    while(s.size()>2){
        string ans="";

        for(int i= 0;i<s.size()-1;i++){
            s[i] =((((int)(s[i])) + (int)(s[i+1])) % 10);
        }
    }
    
    return s[0]==s[1];
}

int main() {
    bool ans = hasSameDigits("34789");
    if(ans){
        cout<<"true"<<endl; 
    }else{
        cout<<"false"<<endl;    
    }
    
    return 0;
}
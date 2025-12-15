#include<bits/stdc++.h>
using namespace std;

string replaceSpaces(string &str ){

    string temp="";

    for(int i=0;i<str.length();i++){
        if(str[i]==' '){
            temp.push_back('@');
            temp.push_back('4');
            temp.push_back('0');

        }else{
            temp.push_back(str[i]);
        }
        
    }
    return temp;
}

int main() {
    string s= "this is a check";
    cout<<replaceSpaces(s);
    return 0;
}
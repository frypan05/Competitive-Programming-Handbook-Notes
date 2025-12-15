#include<bits/stdc++.h>
using namespace std;

bool valid(char ch){
    if((ch>='a'&&ch<='z')||(ch>='A'&&ch<='Z')||(ch>='0'&&ch<='9')){
        return 1;
    }
    return 0;
}

char toLowerCase(char ch){
    if((ch>='a'&&ch<='z')||(ch>='0'&&ch<='9')){
        return ch;
    }else{
        char temp = ch +'a'-'A';
        return temp;
    }
}

bool checkpalindrome(string a){

    int s =0;
    int e=a.length()-1;
    while(s<e){
        if((a[s])!=(a[e])){
            return 0;
        }else{
            s++;
            e--;
        }
    }
    return 1;
}


bool isPalindrome(string s){
    //faltu charcter hatade

    
    string temp = "";
    for(int j=0;j<s.length();j++){
        if(valid(s[j])){
            temp.push_back(s[j]);
        }
    }

    //lowercase me kardo

    for(int j=0;j<temp.length();j++){
        temp[j]= toLowerCase(temp[j]);

    }

    return checkpalindrome(temp);

}

int main() {
    string s = "race a car";
    cout<<isPalindrome(s)<<endl;
    string str = "A man, a plan, a canal: Panama";
    cout<<isPalindrome(str)<<endl;
    string empty = " ";
    cout<<isPalindrome(empty)<<endl;
    return 0;
}
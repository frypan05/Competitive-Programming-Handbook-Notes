#include<bits/stdc++.h>
using namespace std;

//never use "" around parenthesis like "{","[","(" , as it creates a string literal, which is a pointer to a sequence of characters
//The compiler was telling you that you can't directly compare a char variable (like ch or top) with a const char* pointer.

bool matches(char top , char ch){
    if(top == '(' && ch==')'){
        return true;
    }
    else if(top == '{' && ch=='}'){
        return true;
    }
    else if(top == '[' && ch==']'){
        return true;
    }
    return false;
}

bool isValidParenthesis(string s)
{
    stack<char> str;
    
    for(int i =0;i<s.length();i++){
        char ch =s[i];

        //if opening bracket, stack push
        //if closing bracket, stack top check and pop
        if(ch == '(' || ch =='{'|| ch=='['){
            str.push(ch);
        }else{
            //for closing bracket
            if(!str.empty()){
                char top = str.top();
                if(matches(top,ch)){
                    str.pop();
                }else{
                    return false;
                }
            }else{
                return false;
            }
        }     
    }
    if(str.empty()){
        return true;
    }else{
        return false;
    }
}

int main() {
    
    return 0;
}
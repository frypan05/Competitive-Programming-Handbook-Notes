#include<bits/stdc++.h>
using namespace std;

//2 pointer approach

bool checkPalindrome(string& str, int i , int j){

    //base case 
    if(i>j)
        return true;


    if(str[i]!=str[j]){
        return false;
    }else{

        //recursive call
        return checkPalindrome(str,i+1,j-1);
    }
}


//single pointer approach

bool checkPalindrome2(string& str, int i ){

    int j = str.length();

    //base case 
    if(i>j)
        return true;


    if(str[i]!=str[j-1-i]){
        return false;
    }else{

        //recursive call
        return checkPalindrome2(str,i+1);
    }
}


int main() {

    string name = "abba";
    cout<<endl;

    //bool isPalindrome = checkPalindrome(name , 0 ,name.length()-1);
    bool isPalindrome = checkPalindrome2(name , 0 );

    if(isPalindrome){
        cout<< "its a palindrome "<<endl;

    }else{
        cout<<" its not a palindrome"<<endl;
    }

    return 0;
}

//https://www.geeksforgeeks.org/problems/maximum-occuring-character-1587115620/1

#include<bits/stdc++.h>
using namespace std;

char getMaxOccChar(string s){

    int arr[26]={0};

    //create an array of count of charcters
    for(int i=0;i<s.length();i++){
        char ch = s[i];
        

        //lowercase
        int number=0;
        if(ch>='a' && ch<='z'){
            number = ch-'a';
        }
        else{//uppercase
            number = ch-'A';
        }
        arr[number]++;
    }

    //finad max occuring character
    int maxi =-1,ans =0;

    for(int i=0;i<26;i++){
        if(maxi<arr[i]){
            ans = i;
            maxi=arr[i];
        }
    }

    char final = 'a'+ans;
    return final;
}

int main() {
    
    string s;
    //cin>>s;
    getline(cin,s);
    
    cout<<getMaxOccChar(s)<<endl;

    return 0;
} 
#include<bits/stdc++.h>
using namespace std;

int compress(vector<char>&chars){
    int i=0;
    int ansIndex=0;
    int n=chars.size();

    while(i<n){
        int j=i+1;
        while(j<n && chars[i]==chars[j]){
            j++;
        }
        //yaha tab aaoge jab
        //ya toh vectorn pura traverse kardiya
        //ya fir new/different charcter encounter kia a 

        //old character store karlo
        chars[ansIndex++]=chars[i];
        int count=j-i;
        if(count>1){
            //converting counting into singkle digit and solving in answeer
            string cnt = to_string(count);
            for(char ch:cnt){
                chars[ansIndex++]=ch;
            }
        }
        //moving to new /different charcter
        i=j;

    }
    return ansIndex;

}

int main() {

    
    
    return 0;
}
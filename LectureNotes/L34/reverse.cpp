#include<bits/stdc++.h>
using namespace std;

//2pointer approach

void reverse(string& s,int i,int j ){

    cout<<"call received for "<< s<<endl;

    //base case
    if(i>j)
        return ;
    
    swap(s[i],s[j]);

    //recursive call
    reverse(s,++i,--j);

}


//by reusing single pointer
void reverse2(string& s,int i ){

    cout<<"call received for "<< s<<endl;
    int n =s.length();

    //base case
    if(i>n-1-i)
        return ;
    
    swap(s[i],s[n-1-i]);
    i++;

    //recursive call
    reverse2(s,i);

}

int main() {

    string s ="abcde";
    cout<<endl;
  
    //reverse(s,0,s.length()-1);
    reverse2(s,0);
    cout<<endl;

    cout<<s<<endl;
    return 0;
}
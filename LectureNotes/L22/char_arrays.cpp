#include<bits/stdc++.h>
using namespace std;

char toLowerCase(char ch){
    if(ch>='a'&&ch<='z'){
        return ch;
    }else{
        char temp = ch +'a'-'A';
        return temp;
    }
}


bool checkpalindrome(char a[], int n){

    int s =0;
    int e=n-1;
    while(s<e){
        if(toLowerCase(a[s])!=toLowerCase(a[e])){
            return 0;
        }else{
            s++;
            e--;
        }
    }
    return 1;
}

void reverse(char name[],int n){
    int s =0;
    int e=n-1;
    while(s<e){
        swap(name[s++],name[e--]);    
    }
}

int getlength(char name[]){
    int count =0;
    for(int i = 0;name[i]!='\0';i++){
        count++;
    }
    return count;

}

int main() {

    char name[20];

    cout<<"enter your name "<<endl;
    cin>>name;

    //name[2]='\0';

    cout<<"your name is ";
    cout<<name<<endl;

    int len =getlength(name);
    cout<<getlength(name)<<endl;
    reverse(name,len);
    cout<<name<<endl;

    char a[] ="abcba";
    int k =getlength(a);

    cout<<checkpalindrome(a,k)<<endl;
    cout<<" CHARACTER is "<< toLowerCase('b')<<endl;
    cout<<" CHARACTER is "<< toLowerCase('C')<<endl;

    string s ="abcba";
    cout<<s<<endl;
    cout<<s.size();

    

    return 0;
} 
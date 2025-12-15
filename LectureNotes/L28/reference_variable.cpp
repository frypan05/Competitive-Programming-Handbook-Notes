#include<bits/stdc++.h>
using namespace std;

int& func(int a){
    int num = a;
    int &ans =num;
    return ans; 
}

int * fun(int n){
    int * ptr =&n;
    return ptr;
}

void update(int n){
    n++;
}

void update2(int &n){
    n++;
}

int main() {
    
    /*
    int i=5;


    //create a reference variable
    int &j = i;

    cout<<i<<endl;
    i++;
    cout<<i<<endl;
    j++;
    cout<<i<<endl;
    cout<<j<<endl;
    */

    int  n=5;
    cout<<"before "<<n<<endl;
    update2(n);
    cout<<"after "<<n<<endl;

    func(n);

    fun(n);

    return 0;
}
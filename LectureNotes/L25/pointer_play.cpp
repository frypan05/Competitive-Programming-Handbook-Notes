#include<bits/stdc++.h>
using namespace std;

int main() {

    //pointer to int is created pointing to garbage address
    //int *p;
    //cout<<*p;
    //if it points to address in read only memory system (where we should update) , this is dangerous
    
    
    //int *p=0; //null pointer
    //cout<<*p<<endl;  //dereferencing a null pointer

    
    /*
    int i=5;
    int *q = &i;
    cout<<q<<endl;
    cout<<*q<<endl;

    int *p =0;
    p = &i;
    cout<<p<<endl;
    cout<<*p<<endl;*/


    /*
    int num=5;
    //int a= num;
    //a++;

    int *p = &num;
    int a = *p;
    a++;
    */

 
    int num=5;
    int *p = &num;
    cout<<"before "<<num<<endl;
    (*p)++;
    cout<<"after "<<num<<endl;

    //copyig a pointer
    int *q= p;
    cout<< p<<" - "<<q<<endl;
    cout<< *p<<" - "<<*q<<endl;

    //important concept
    //*t++ and (*t)++ are same

    int i=3;
    int *t= &i;

    *t = *t+1;
    cout<<*t<<endl;
    cout<<" before t " <<t<<endl;
    t=t+1;
    cout<<" after t " <<t<<endl;

    return 0;
}
#include<bits/stdc++.h>
using namespace std;

int main() {

    int num=5;
    cout<<num<<endl;

    //address of operator - &

    cout<<" address of num is "<< &num<<endl;

    int *ptr = &num;
    cout<<"value is "<<*ptr<<endl;
    cout<<"address is "<<ptr<<endl;

    double d = 4.3;
    double *p2 = &d;

    cout<<"value is "<<*p2<<endl;
    cout<<"address is "<<p2<<endl;

    cout<<" size of integer is "<<sizeof(num)<<endl;
    cout<<" size of pointer is "<<sizeof(ptr)<<endl;

    cout<<" size of pointer is "<<sizeof(p2)<<endl;

    int *pn  =NULL;
    cout<<pn<<endl;
    cout<<*pn<<endl;


    return 0;
}
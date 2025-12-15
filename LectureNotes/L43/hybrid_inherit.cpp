#include<bits/stdc++.h>
using namespace std;

//hybrid of multiple and hierarchical

class A{

    public:
    void funcA(){
        cout<<"inside function A"<<endl;
    }
};

class D{
    public:
    void funcD(){
        cout<<"inside function D"<<endl;
    }
};

class B: public A{

    public:
    void funcB(){
        cout<<"inside function B"<<endl;
    }
};

class C: public A,public D{

    public:
    void funcC(){
        cout<<"inside function C"<<endl;
    }
};




int main() {


    
    return 0;
}
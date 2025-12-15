#include<bits/stdc++.h>
using namespace std;

class A{

    public:
    void func(){
        cout<<" i am A "<<endl;
    }
};

class B{

    public:
    void func(){
        cout<<" i am B "<<endl;
    }
};

class C: public A, public B{

};



int main() {

    C obj;

    //obj.func();//ambiguos

    obj.A::func();//function call from A
    obj.B::func();//function call from B


    return 0;
}
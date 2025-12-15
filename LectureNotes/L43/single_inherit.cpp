#include<bits/stdc++.h>
using namespace std;

class animal{

    public:
    int age;
    int weight;

    public:
    void speak(){
        cout<<"speaking "<<endl;
    }

};


class dog:public animal{

};


int main() {

    dog d;
    d.speak();
    cout<<d.weight<<endl;
    cout<<d.age<<endl;

    
    return 0;
}
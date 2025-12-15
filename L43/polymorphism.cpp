#include<bits/stdc++.h>
using namespace std;

class A{    
    public: 
    void sayHello(){
        cout<<"hello "<<endl; 
    }
    int sayHello(string name,int n){
        cout<<"hello "<<endl; 
        return n;
    }
    void sayHello(string name){
        cout<<"hello "<<name<<endl; 
    }

};

class B{
    
    public:
    int a;
    int b;

    public:
    int add(){
        return a+b;
    }


    //operator overloading
    void operator+(B &obj){
        //int value1 = this->a;
        //int value2 = obj.a; 
        //cout<<"output "<< value2 - value1<<endl;

        cout<<"hello babbar "<<endl;
    }

    void operator()(){
        cout<<"mai bracket hu "<<this->a<<endl;
    }


};


class animal{
    public:
    void speak(){
        cout<<" speaking "<<endl;
    }

};

class dog: public animal{
    public:
    void speak(){
        cout<<" barking "<<endl;
    }

};
 






int main() {




    dog obj;
    obj.speak();



 









    // B obj1,obj2;

    // obj1.a =4;
    // obj2.a = 7;

    // obj1+obj2;


    // obj1(); 





    // A obj;  
    // obj.sayHello();
    
    return 0;
}
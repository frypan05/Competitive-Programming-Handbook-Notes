#include<bits/stdc++.h>
using namespace std;

class human{
    public:
    int height;
    protected:
    int weight;
    private:
    int age;

    int getAge(){
        return this-> age;
    }
    void setWeight(int w){
        this-> weight =w;
    }

};

//class male: public human{
//
//    public:
//    string color;
//
//    void sleep(){
//        cout<<"male sleeping ";
//    }
//
//
//};

//class male: protected human{
//
//    public:
//    string color;
//
//    void sleep(){
//        cout<<"male sleeping ";
//    }
//
//    int getHeight(){
//        return this-> height;
//    }
//
//};

class male: private human{

    public:
    string color;

    void sleep(){
        cout<<"male sleeping ";
    }

    int getHeight(){
        return this-> height;
    }

};

int main() {


    male m1;
    //cout<<m1.height<<endl; 
    cout<<m1.getHeight()<<endl;//only way to access private/protected data members of child class




  
 

    // male object1;

    // cout<<object1.age<<endl;
    // cout<<object1.height<<endl;
    // cout<<object1.weight<<endl; 

    // cout<<object1.color<<endl;
    // object1.setWeight(85);
    // cout<<object1.weight<<endl;
    // object1.sleep();

    
    return 0;
} 
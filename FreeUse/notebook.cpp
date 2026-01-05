/*
//Accessor and mutator.
#include <bits/stdc++.h>
using namespace std;

class Marker{
    private:
        string color;
        string nature;
    public: //default constructor / Non Parameterized constructor.
        marker(){
            color = "Red";
            nature = "Temporary";
        }
        //parameterized constructor.
        marker(string color = "Red", string nature = "Temporary"){
            this -> color = color;
            this -> nature = nature;
        }

        //copy constructor
        Marker(&m){
            this -> color = m.color;
            this -> nature = m.nature;
        }
        pair<string, string> getter(){
            return {color, nature};
        }
}
int main(){
  Marker m1("Blue", "Temporary"), m2("black", "Permanent");
}

*/


#include<bits/stdc++.h>
using namespace std;

class A{
    private:
    int a,b;

    public:
    A(int x, int y){
        a=x, b = y;
    }
    int Multiplication();
};

int A::Multiplication(){
    return a*b;
}

int main(){
    A obj(5,6);
    cout<< obj.Multiplication();
    return 0;
}

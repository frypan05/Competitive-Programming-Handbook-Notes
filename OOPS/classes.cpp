#include <iostream>
#include <string>
using namespace std;

class Car{
    private:
    //Attributes
    string brand;
    string model;
    int speed;

    public:
    //Constructor
    Car(const string& brand, const string& model)
    : brand(brand), model(model), speed(0){
    }
    //method1
    void accelerate(int increment){
        speed += increment;
    }
    //method2
    void displayStatus() const {
        cout << brand << " is running at " << speed << "km/h" << endl;
    }
};

int main(){
    Car corolla ("toyota", "corolla");
    Car mustang ("ford", "mustang");

    corolla.accelerate(20);
    mustang.accelerate(50);

    corolla.displayStatus();
    mustang.displayStatus();
    return 0;
}

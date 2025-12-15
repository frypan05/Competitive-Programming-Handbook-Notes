#include<bits/stdc++.h>
//#include"hero.cpp"

//https://www.naukri.com/code360/guided-paths/basics-of-c/content/118817/offering/1382190

using namespace std;


class hero{
    

    //properties
    //char name[100];

    private:
    int health;

    //
    public:
    static int timeToComplete;
    char *name;
    char level;

    hero(){
        cout<<"simple constructor called"<<endl;
        name = new char[100];
    }


    //parameterised constructor
    hero(int health){
        //cout<<"this "<<this <<endl;
        this -> health= health;
    }
    hero(int health,char level){
        //cout<<"this "<<this <<endl;
        this -> level= level;
        this -> health= health;
    }

    //copy constructor
    hero(hero& temp){

        char *ch= new char[strlen(temp.name)+1];
        strcpy(ch,temp.name);
        this -> name = ch;

        cout<<"copy constructor called "<<endl;
        this -> level= temp.level;
        this -> health= temp.health;  
    }


    void print(){
        cout<<"name is "<<this->name<<",";
        cout<<"health is "<<this->health<<",";
        cout<<"level is "<<this->level<<endl;
    }

    int getHealth(){
        return health;
    }
    char getLevel(){
        return level;
    }
    void setHealth(int h){
        health = h;
    }
    void setLevel(char ch){
        level = ch;
    }
    void setName(char name[]){
        strcpy(this->name,name);
    }


    static int random (){
        //cout<<this->health<<endl;
        //cout<<health<<endl;
        //return health;
        return timeToComplete;
    }


    ~hero(){ //destructor
        cout<<"destructor bhai called"<<endl;
    }

};

int hero :: timeToComplete = 5;
 
int main() {

    //cout<<hero::timeToComplete<<endl;

    cout<<hero::random()<<endl;





    //  hero a;
    //  cout<<a.timeToComplete<<endl; 
    //  //not recommended as it does not belong to objec
    //  hero b;
    //  b.timeToComplete=10;//updated due to b
    //  cout<<a.timeToComplete<<endl; 
    //  cout<<b.timeToComplete<<endl; 

 

    // //static
    // hero a;


    // //dynamic
    // hero *b = new hero;
    // delete b; //manually calling destructor for dynamic allocation





    // hero hero1;

    // hero1.setHealth(12);
    // hero1.setLevel('D');
    // char name[7]="babbar";
    // hero1.setName(name);
    // //hero1.print();

    // //use default copy constructor
    // hero hero2(hero1);
    // //hero hero2 = hero1;
    // //hero2.print();

    // hero1.name[0]= 'G';
    
    // hero1.print();
    // hero2.print();

    // hero1=hero2; //copy assignment operator

    // hero1.print();
    // hero2.print();


/*

    hero S(70,'C');
    S.print();


    //copy constructor
    hero R(S);
    //R.health=suresh.health;
    //R.level = suresh.level;
    R.print();
*/


/*
    //hero tt;

    //object created statically
    hero ramesh(10);
    //cout<<"address of ramesh "<<&ramesh<<endl;
    ramesh.print();


    //dynamically
    hero *h = new hero(11);
    h->print();


    hero temp(22,'B');
    temp.print();

*/


    /*
    //static allocation
    hero a;
    a.setLevel('B');
    a.setHealth(90);
    cout<<" level is "<<a.level<<endl;
    cout<<" health is "<<a.getHealth()<<endl;

    //dynamically
    hero *b  = new hero;
    b->setLevel('A');
    b->setHealth(80);
    cout<<" level is "<<b->level<<endl;
    cout<<" health is "<<(*b).getHealth()<<endl;
*/
     

    // //creation of object
    // hero ramesh; 

    // //use getter to access private data members
    // cout<<" ramesh health is "<<ramesh.getHealth()<<endl;
    // //use setter to modify private data members
    // ramesh.setHealth(70);
    // //ramesh.health=70;
    // ramesh.level='A';
    

    // //cout<<"size "<<sizeof(h1);

    // //cout<<"health is "<<ramesh.health<<endl;
    // cout<<" ramesh health is "<<ramesh.getHealth()<<endl;
    // cout<<"level is "<<ramesh.level<<endl;

    // cout<<sizeof(ramesh);


    return 0;
}
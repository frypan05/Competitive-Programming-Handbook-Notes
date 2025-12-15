#include<bits/stdc++.h>
using namespace std;

int score = 5;

void b(int& i){
    cout<<i <<endl;
    cout<<"Score"<<score<<endl;

    score++;
    
}

void a(int & i){

    cout<<"Score"<<score<<endl;
    cout<<i<<endl;
    b(i);

}

int main() {

    int i=5;
    a(i);
    cout<<"Score"<<score<<endl;
    
    return 0;
}
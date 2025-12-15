#include<bits/stdc++.h>
using namespace std;

int factorial(int n){

    //base case
    if(n==0){
        return 1;
    }

    //int chotti_problem= factorial(n-1);
    //int badi_problem = n*chotti_problem;
    //return badi_problem;
    return n*factorial(n-1);
    
}

int powerof2(int n){
    if(n==0){
        return 1;
    }
   
    return 2*powerof2(n-1);
    
}

void counting(int n){
    //base case
    if(n==0){
        return ;
    }

    cout<<n<<endl;

    //recursive relation - tail 
    counting(n-1);
    
}

void counting2(int n){
    //base case
    if(n==0){
        return ;
    }
    
    //recursive relation -head
    counting2(n-1);
    
    cout<<n<<endl;


}


int main() {
    int n = 7;
    cout<<factorial(n)<<endl;
    cout<<powerof2(n)<<endl;;

    counting(n);//tail recursion
    counting2(n);//head recursion

    return 0;
}
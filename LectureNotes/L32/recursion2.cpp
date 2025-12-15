#include<bits/stdc++.h>
using namespace std;

void reachHome(int src, int dest){

    cout<<" source "<<src<<" destination "<< dest<<endl;
    //base case
    if(src==dest){
        cout<<"ghar pohoch gya"<<endl;
        return ;
    }

    //processing = ek step aage badh jao
    src++;

    //recursive call
    reachHome(src,dest);

}

int fibonacci(int n){

    //base case
    if(n==0||n==1){
        return n;
    }
    
    return fibonacci(n-1)+fibonacci(n-2);
}


int fibonacci2(int n){
    //loop
}

int fibonacci3(int n){
    //Binet's Nth-term Formula

    double phi = (sqrt(5) + 1) / 2;     
    return round(pow(phi, n) / sqrt(5));

}

int climbStairs(int n) {

    if(n<0){
        return 0;
    }
    if(n==0){
        return 1;
    }

    int ans = climbStairs(n-1)+climbStairs(n-2);
        
}

void saydigit(int n, string arr[]){

    //base case
    if(n==0){
        return ;
    }

    //processing
    int digit = n%10;
    n=n/10;
    

    //recursive call
    saydigit(n,arr);
    cout<< arr[digit]<<" ";

}


int main() {

    //int dest= 10;
    //int src = 1;
//
    //cout<<endl;
    //reachHome(src,dest);

    int n=41;
    int ans = fibonacci(n);
    cout<<ans<<endl;

    //cout<<climbStairs(n);

    cout<<endl<<endl;


    string arr[10]={"zero","one","two","three","four","five","six","seven","eight","nine"};

    saydigit(n,arr);
    cout<<endl;
    
    return 0;
}
#include<bits/stdc++.h>
using namespace std;

    bool isPrime(int n){
        if(n<=1){
        return false;
    }
    
    for(int i=2;i<n;i++){
        if(n%i==0){
            return false;
        }
    }
    return 1;
}

// brute force = T.C of O(n*2) appraoch 
//int countPrimes(int n){
//       int cnt=0;
//    for(int i=2;i<n;i++){
//        if(isPrime(i)){
//            cnt++;
//        }
//    }
//    return cnt;
//}


//sieve of frastothenes

int countPrimes(int n){
    int cnt=0;
    vector<bool> prime(n+1,true);
    prime[0] = prime[1] =false;
    
    for(int i = 2;i<n;i++){
        if(prime[i]){
            cnt++;

            for(int j = 2*i;j<n;j=j+i){
                prime[j]=0;
            }
        }
    }
    return cnt;
}


int main() {

    int n=40;
    cout<<countPrimes(n);
    
    return 0;
}


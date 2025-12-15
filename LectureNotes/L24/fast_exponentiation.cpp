#include<bits/stdc++.h>
using namespace std;

int modularExponentiation(int x,int n, int m){
    int res=1;
    while(n>0){
        if(n&1){//odd
            res = (1LL*(res)*(x)%m)%m;
        }
            x=(1LL*(x)%m*(x)%m)%m;
            //1LL for long long typecasting
            n=n>>1;
        
    }
    return res;
}

int main() {
    int x = 2, n =10, m = 5;
    cout<<modularExponentiation(x,n,m);
    
    return 0;
}
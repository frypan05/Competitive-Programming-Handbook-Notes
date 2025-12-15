#include<bits/stdc++.h>
using namespace std;



int main() {
    int num=28;
      int sum=1;
        for(int i=2;i<=num/2;i++){
             if(num%i==0){
                sum+=i;
                
                cout<<i<<endl;}
                
        }

        cout<<sum;

    return 0;
}
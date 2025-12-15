#include <iostream>
using namespace std;

//sqrt using binary search

long long int sqrt(int x){
    int s=0;
    int e=x;
    int ans;
    long long int mid= s + (e-s)/2;

    while(s<=e){
        long long int square = mid*mid;
        if(square==x){
            return mid;
        }
        else if(square>x){
            e=mid-1;
        }
        else if(square<x){
            ans=mid;
            s=mid+1;
        }
        mid= s+(e-s)/2;
    }
    return ans;
}

double moreprecise(int n ,int precision, int tempsol){
    double factor = 1;
    double ans =tempsol;

    for(int i=0;i<precision;i++){
        factor = factor/10;
        for(double j=ans;j*j<n; j+=factor){
            ans=j;
        }
    }
    return ans;
}

int main(){

    int x=50;
    int tempsol = (sqrt(x));
    cout<<moreprecise(x,3,tempsol);
    return 0;
}


//newton's method 

//  long r = x;
//   while (r*r > x){
//       r = (r + x/r) / 2;}
//   return r;
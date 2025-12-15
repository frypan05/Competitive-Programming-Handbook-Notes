#include <iostream>
using namespace std;

//O(n) approach

//bool square(long num){
//        for(long i=0;i<=num;i++){
//        if(i*i==num){
//            return true;
//        }      
//    }return false;
//}
//
//int main() {
//    long num=37;
//    cout<<square(num);
//    return 0;
//}

//O(logn) approach

bool square(int n){

    int start =0;
    long end =n;
    long mid = start + (end-start)/2;


    while(start<=end){
        long square = mid*mid;

        if(square==n){
            return true;
        }
        
        if(square<n){
            start=mid+1;

        }else{
            end =mid-1;
        }

        mid=start+(end-start)/2;
    }
    return false;
}

int main() {
    int n =16;
    cout<<square(n);
    
    return 0;
}
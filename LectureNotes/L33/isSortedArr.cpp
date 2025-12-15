#include<bits/stdc++.h>
using namespace std;

bool isSorted(int arr[],int n){

    //base case
    if(n<2){
        return true;
    }
   
    if(arr[0]>arr[1]){
        return false;
    }else{
        bool remainingPart = isSorted(arr+1,n-1);
        return remainingPart;
    }
}


int main() {
    
    int arr[7]={2,4,6,9,11,13,3};
    int n = sizeof(arr)/sizeof(arr[0]);

    cout<<isSorted(arr,n);



    return 0;
}
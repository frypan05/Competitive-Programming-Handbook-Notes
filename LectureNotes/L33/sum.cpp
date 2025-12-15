#include<bits/stdc++.h>
using namespace std;

int sumArr(int arr[],int n){

    if(n==0){
        return 0;
    }    
    if(n==1){
        return arr[0];
    }


    int remainingPart = sumArr(arr+1,n-1); 

    int sum=0;
    sum=arr[0]+ remainingPart;
    return sum;

}

int main() {

    int arr[5]={1,4,16,9,25};
    int size= 5;

    cout<<sumArr(arr,size);
    
    
    return 0;
}
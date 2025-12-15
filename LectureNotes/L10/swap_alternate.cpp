#include<iostream>
using namespace std;

void swap_alt(int arr[],int n){

    for(int i=0;i<n;i+=2){
        if(i+1<n){
            arr[i]=arr[i]+arr[i+1];
            arr[i+1]=arr[i]-arr[i+1];
            arr[i]=arr[i]-arr[i+1];      
            //swap(arr[i],arr[i+1]);      
        }

    }
}

void printarray(int arr[],int n){
    for(int i=0;i<n;i++){
        cout<<arr[i]<<endl;
    }
}

int main(){

    int arr[]={1,2,3,4,5,6,7,8};
    int n = sizeof(arr)/sizeof(arr[0]);
    
    swap_alt(arr,n);
    printarray(arr,n);

    return 0;
}
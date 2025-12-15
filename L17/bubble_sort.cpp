#include <bits/stdc++.h>
using namespace std;

void bubbleSort(int arr[],int n){

    for(int i=1;i<n;i++){
        //for round 1 to n-1
        bool swapped =false;
        for(int j=0;j<n-i;j++){
            if(arr[j]>arr[j+1]){
                swap(arr[j],arr[j+1]);
                swapped=true;
            }  
        }
        if(swapped==false){
            //already sorted
            break;
        }   
    }
}

void printSort(int arr[],int n){
    bubbleSort(arr,n);
    for(int i=0;i<n;i++){
        cout<<arr[i]<<endl;
    }
}

int main(){
    int arr[6]={10,1,7,6,14,9};
    int n=6;
    printSort(arr,n);
    return 0;
}
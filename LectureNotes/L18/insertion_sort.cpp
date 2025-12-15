#include<bits/stdc++.h>
using namespace std;

void insertionSort(int arr[],int n){
    for(int i=1;i<n;i++){
        int temp=arr[i];
        int j= i-1;
        for(;j>=0;j--){
            if(arr[j]>temp){
                //shift
                arr[j+1]=arr[j];
            }else{//ruk jao
                break;
            }
        }
        arr[j+1]=temp;
    }
}

//implement with while loop  

void printSort(int arr[],int n){
    insertionSort(arr,n);
    for(int i=0;i<n;i++){
        cout<<arr[i]<<endl;
    }
}

int main() {
    int arr[7]={10,1,7,4,8,2,11};
    int n=7;
    printSort(arr,n);
    return 0;
}
#include<bits/stdc++.h>
using namespace std;

void moveZeroes(int* arr, int size){

    int i=0;

    for(int j=0;j<size;j++){
            if(arr[j]!=0){
            swap(arr[i],arr[j]);
            i++;
        }
    }

}


int main() {

    int arr[6]={1,0,1,0,3,12};
    int size=6;

    moveZeroes(arr,size);

    for(int i=0;i<size;i++){
        cout<<arr[i]<<" ";
    }
    
    return 0;
}
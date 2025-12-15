#include<bits/stdc++.h>
using namespace std;

void reverse(int* arr,int size){

    int s=0;
    int e = size-1;

    while(s<e){
        swap(arr[s],arr[e]);
        s++;
        e--;
    }
    
}

int main() {
    
    int arr[5]={1,6,3,5,23};
    int size =5;

    reverse(arr,size);

    for(int i=0;i<size;i++){
        cout<<arr[i]<<" ";
    }

    
    return 0;
}
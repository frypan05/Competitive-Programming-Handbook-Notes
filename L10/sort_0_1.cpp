#include<bits/stdc++.h>
using namespace std;

void sort01(int* arr,int size){
    int s=0;
    int e=size-1;
    


    while(s<e){

        while(arr[s]==0 &&s<e){
            s++;
        }
        while(arr[e] == 1 &&s<e){
            e--;
        }


        //agar yha pohoch gye iska matlab
        //arr[s]==1 and arr[e]==0;
        
        if(s<e){
            swap(arr[s],arr[e]);
            s++;
            e--;
        }
        
        
    }
    
}

int main() {

    int arr[13]={0,0,1,0,0,1,0,1,1,1,0,0,0};
    int size=13;

    sort01(arr,size);    

    for(int i=0;i<size;i++){
        cout<<arr[i]<<endl;
    }
    return 0;
}
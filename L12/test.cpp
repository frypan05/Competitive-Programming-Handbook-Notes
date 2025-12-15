#include<iostream>
using namespace std;

int max(int arr[],int size){
    int max=0;
  
    for(int i =0;i<size;i++){
        
        if(arr[i]>max ){
            
            
            max=arr[i];
        }
    }
   return max; 
}

int min(int arr[],int size){
    int min=0;
    
    for(int i =0;i<size;i++){
        
        if(arr[i]<min ){
            
            
            min=arr[i];
        }
    }
   return min; 
}


int bs1(int arr[],int size){
    int key=max(arr,size);
    int start=0;
    int end = size-1;
    int mid = (start+end)/2;

    while(start<=end){
        if(arr[mid]==key){
            return mid;
        }
        if(key>arr[mid]){
            start =mid+1;

        }else{
            end=mid-1;
        }
        mid=start+(end-start)/2;
    }
    
}
int bs2(int arr[],int size){
    int key=min(arr,size);
    int start=0;
    int end = size-1;
    int mid = (start+end)/2;

    while(start<=end){
        if(arr[mid]==key){
            return mid;
        }
        if(key>arr[mid]){
            start =mid+1;

        }else{
            end=mid-1;
        }
        mid=start+(end-start)/2;
    }
    
} 

int sort();

int main(){
    int arr[10]={1,2,3,4,5,6,254,73,342,9};
    cout<<max(arr,10)<<endl;
    cout<<bs1(arr,10)<<endl;
    cout<<min(arr,10)<<endl;
    cout<<bs2(arr,10);
    return 0;
}




int sort(int arr,int size){
    
}
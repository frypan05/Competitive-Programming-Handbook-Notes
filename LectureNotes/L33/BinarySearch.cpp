#include<bits/stdc++.h>
using namespace std;



bool binarysearch(int arr[],int size,int key){
    if(size==0){
        return false;
    }

    int s=0;
    int e = size-1;
    int mid = ((e-s)/2)+s;

    if(key==arr[mid]){
        return true;
    }else if(arr[mid]<key){
        binarysearch(arr+(size/2),size/2,key);
    }else if(arr[mid]>key)
        binarysearch(arr,size/2,key);

}

void print(int arr[],int s, int e){
    for(int i=s;i<=e;i++){
        cout<<arr[i]<<" ";
    }cout<<endl;
}

bool binarysearch2(int *arr,int s,int e,int k){
    cout<<endl;
    print(arr,s,e);
    //base case
    //element not found
    if(s>e)
        return false;

    int mid = s+(e-s)/2;

    //element found
    if(arr[mid]==k)
        return true;
    
    if(arr[mid]<k){
        return binarysearch2(arr,mid+1,e,k);
    }
    else{
        return binarysearch2(arr,s,mid-1,k);
    }
}

int main() {

    int arr[6]={2,4,6,10,14,18};

    if(binarysearch(arr,6,10)){
        cout<<"found"<<endl;
    }else{
        cout<<"not found"<<endl;
    }


    int key =18;

    cout<<"present or not "<<binarysearch2(arr,0,5,18);
    
    return 0;
}
 

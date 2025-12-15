//same as book allocation

#include <iostream>
using namespace std;

bool isPossible(int arr[],int n,int m,int mid){
    int painterCount = 1;
    int unitBoard = 0;

    for(int i=0;i<n;i++){
        if(unitBoard + arr[i]<=mid){
            unitBoard+=arr[i];
        }else{
            painterCount++;
            if(painterCount>m || arr[i]>mid){
                return false;
            }
            unitBoard=arr[i];

        }
    }
    return true;
}

int binarysearch(int arr[],int n,int m){
    int ans=-1;
    int s =0;
    int sum=0;

    for(int i=0;i<n;i++){
        sum+= arr[i];
    }
    int e = sum;
    int mid = s+(e-s)/2;

    while(s<=e){
        if(isPossible(arr,n,m,mid)){
            ans=mid;
            e=mid-1;
        }else{
            s=mid+1;
        }
        mid = s+(e-s)/2;

    }
    return ans;
}

int main() {
    int arr[6]={2,1,5,6,2,3};
    int k =2;
    cout<<binarysearch(arr,6,2);
    return 0;
}
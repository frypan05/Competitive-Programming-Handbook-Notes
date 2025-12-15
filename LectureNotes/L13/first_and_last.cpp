//Problem statement
//You have been given a sorted array/list 'arr' consisting of ‘n’ elements. You are also given an integer ‘k’.
//
//
//
//Now, your task is to find the first and last occurrence of ‘k’ in 'arr'.
//
//
//
//Note :
//1. If ‘k’ is not present in the array, then the first and the last occurrence will be -1. 
//2. 'arr' may contain duplicate elements.
//
//
//Example:
//Input: 'arr' = [0,1,1,5] , 'k' = 1
//
//Output: 1 2
//
//Explanation:
//If 'arr' = [0, 1, 1, 5] and 'k' = 1, then the first and last occurrence of 1 will be 1(0 - indexed) and 2.
//
//
//Detailed explanation ( Input/output format, Notes, Images )
//Sample Input 1:
//8 2
//0 0 1 1 2 2 2 2
//
//
//Sample output 1:
//4 7
//
//
//Explanation of Sample output 1:
//For this testcase the first occurrence of 2 in at index 4 and last occurrence is at index 7.

#include<iostream>
using namespace std;

int firstOcc(int arr[],int n,int key){

    int s=0,e=n-1;
    int mid = s+ (e-s)/2;
    int ans = -1;

    while(s<=e){
        if(arr[mid]==key){
            ans=mid;
            e=mid-1;
        }
        else if(key>arr[mid]){//right me jao
            s=mid+1;
        }
        else if(key<arr[mid]){//left me jao
            e=mid-1;
        }
        mid = s + (e-s)/2;
    }
    return ans;
}

int lastOcc(int arr[],int n,int key){

    int s=0,e=n-1;
    int mid = s+ (e-s)/2;
    int ans = -1;

    while(s<=e){
        if(arr[mid]==key){
            ans=mid;
            s=mid+1;
        }
        else if(key>arr[mid]){//right me jao
            s=mid+1;
        }
        else if(key<arr[mid]){//left me jao
            e=mid-1;
        }
        mid = s + (e-s)/2;
    }
    return ans;
}

int main(){

    int even[11]={1,2,3,3,3,3,3,3,3,3,5};
    cout<<firstOcc(even,11,3)<<endl;
    cout<<lastOcc(even,11,3)<<endl;

    return 0;
}

 
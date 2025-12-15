#include<bits/stdc++.h>
using namespace std;


//three pointer

void sort012(int* arr,int size){
     int low = 0, mid = 0, high = size - 1;

    while (mid <= high) {
        if (arr[mid] == 0) {
            
            swap(arr[low], arr[mid]);
            low++;
            mid++;
        } else if (arr[mid] == 1) {
            
            mid++;
        } else { // arr[mid] == 2
            // Swap arr[mid] with arr[high] and decrement high pointer
            swap(arr[mid], arr[high]);
            high--;
        }
    }
     
}



int main() {

    int arr[13]={0,0,1,0,2,1,2,1,2,1,0,0,0,};
    int size=13;

    sort012(arr,size);    

    for(int i=0;i<size;i++){
        cout<<arr[i]<<" ";
    }
    return 0;
}
#include<bits/stdc++.h>
using namespace std;

/*
int sortArr(vector<int>& arr){
  sort(arr.begin(),arr.end());
  return arr[arr.size()-1];
}
*/

int findLargestElement(int arr[], int n){

  int max = arr[0];
  for (int i = 0; i < n; i++){
    if (max < arr[i]){
      max = arr[i];
    }
  }
  return max;
}


int main() {
      
    /*
    vector<int> arr1 = {2,5,1,3,0,8,9};
    cout << "The largest element is: " << sortArr(arr1)<<endl;
    */

    int arr[] = {2,5,1,3,0};
    int n  = 5;
    int max = findLargestElement(arr, n);
    cout << "The largest element is: " << max <<endl;

    return 0;
}


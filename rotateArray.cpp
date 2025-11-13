

#include<bits/stdc++.h>
using namespace std;

void solve(int arr[], int n){
  int temp = arr[0];
  for (int i = 0; i < n-1; i++) {
    arr[i] = arr[i + 1];;
  }
  arr[n-1] = temp;

  for (int i = 0; i < n; i++) {
    cout << arr[i] << " ";
  }
}

int main() {
    int n = 5;

    int arr[] = {1,2,3,4,5};
    solve(arr, n);
    return 0;
}


/*
void rotate(vector<int>& nums, int k){
  int n = nums.size();
  k &= n;

  reverse(nums.begin(),nums.end());
  reverse(nums.begin(), nums.begin() + k);
  reverse(nums.begin() + k, nums.end());
}

*/

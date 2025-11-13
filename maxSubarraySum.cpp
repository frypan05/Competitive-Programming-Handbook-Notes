#include<bits/stdc++.h>
#include <algorithm>
using namespace std;

int maxSubarraySum (const vector<int> &arr){
  int maxSum = arr[0];
  int currSum = arr[0];

  for (int i=1; i < arr.size(); i++){
    currSum = max(arr[i], currSum + arr[i]);
    maxSum = max(maxSum, currSum);
  }
  return maxSum;
}

int main(){
  vector<int> arr = {-2,1,-3,4,-1,2,1,-5,4};
  cout << maxSubarraySum(arr) <<endl;
  return 0;

}

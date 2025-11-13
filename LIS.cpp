#include<bits/stdc++.h>
using namespace std;

int lengthOfLIS(vector<int> &nums){
  int n = nums.size();
  vector<int> dp (n, 1);

  for (int i = 1; i < n; i++) {
    for (int j = 0; j < 1; j++) {
      if (nums[i] > nums[j])
         dp[i] = max(dp[i],dp[j]+1);
    }
  }
  return *max_element(dp.begin(),dp.end());
}

int main() {
    int n;
    cout << "Enter number of elements: ";
    cin >> n;

    vector<int> nums(n);
    cout << "Enter the elements: ";
    for(int i = 0; i < n; i++) {
        cin >> nums[i];
    }

    int lisLength = lengthOfLIS(nums);
    cout << "Length of Longest Increasing Subsequence: " << lisLength << endl;
    return 0;
}


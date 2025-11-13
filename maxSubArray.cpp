#include <bits/stdc++.h>
using namespace std;

vector<int> maxSubArray(const vector<int>& nums){
  int maxSum = nums[0], currSum = nums[0];
  int start = 0, end = 0, tempStart = 0;

  for (int i = 1; i < nums.size(); ++i) {
    if (currSum + nums[i] < nums[i]){
      currSum = nums[i];
      tempStart = i;
    } else {
      currSum += nums[i];
    }

    if (currSum > maxSum) {
      maxSum = currSum;
      start = tempStart;
      end = i;
    }
  }
  return vector<int>(nums.begin() + start, nums.begin() + end + 1);
}


int main () {
  vector<int> nums = {5, 1, 6, 3, 8, 2};
  vector<int> result = maxSubArray(nums);

  cout << " Max Sub Array Elements: ";
  for (int x : result) cout << x << " ";

  cout << endl;
  return 0;
}

#include<bits/stdc++.h>
using namespace std;

class solution {
  public:
    int findMaxConsecutiveOnes(vector<int> & nums){
      int count = 0;
      int maxi = 0;
      for (int i = 0; i < nums.size(); i++) {
        if (nums[i] == 1) {
          count++;
        }
        else {
          count = 0;
        }
      maxi = max(maxi,count);

      }
      return maxi;
    }
};

int main() {
    vector<int> nums = {1, 1, 0, 1, 1, 1};
    solution obj;
    int ans = obj.findMaxConsecutiveOnes(nums);
    cout << "The maximum consicutive 1's are: " << ans;
    return 0;
}
//hehehehe

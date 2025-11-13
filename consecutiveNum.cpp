#include <bits/stdc++.h>
using namespace std;

int maxConsecutiveOnes(const vector<int>& nums) {
    int current = 0, max = 0;
    for (int num : nums) {
        if (num == 1) {
            current++;
            max = std::max(max, current);
        } else {
            current = 0;
        }
    }
    return max;
}

int main() {
    vector<int> nums = {1, 1, 0, 1, 1, 1, 0, 1};
    cout << "Maximum consecutive 1's: " << maxConsecutiveOnes(nums) << endl;
    return 0;
}

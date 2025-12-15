#include<bits/stdc++.h>
using namespace std;

int triangularSum(vector<int>& nums) {
    
    while(nums.size()>1){
        int n= nums.size();
        vector<int> newNums(n-1);
        for (int i =0;i<n-1 ; i++){
            newNums[i] = (nums[i]+nums[i+1])%10;
        }
        nums = newNums;
    }
    return nums[0];
        
}

int main() {

    vector<int> nums1 = {1, 2, 3, 4, 5};

    cout<<triangularSum(nums1);
    
    return 0;
}
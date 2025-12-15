//https://leetcode.com/problems/valid-triangle-number/?envType=daily-question&envId=2025-09-28

#include<bits/stdc++.h>
using namespace std;

int triangleNumber(vector<int>& nums) {
        sort(nums.begin(),nums.end());
        int count =0;
        for( int i = nums.size() -1 ; i>1;i--){
            if(nums[i] + nums[i-1] > nums[i-2] && nums[i-1] + nums[i-2] > nums[i] && nums[i-2] + nums[i] > nums[i-1]){
                for 
            }
            
        }
        return count;
    }

int main() {
    vector<int> nums = {2,2,3,4};
    cout<<triangleNumber(nums);


    return 0;
}
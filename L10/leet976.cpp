//https://leetcode.com/problems/largest-perimeter-triangle/?envType=daily-question&envId=2025-09-28

#include<bits/stdc++.h>
using namespace std;

int largestPerimeter(vector<int>& nums) {
        sort(nums.begin(),nums.end());
        for( int i = nums.size() -1 ; i>1;i--){
            if(nums[i] + nums[i-1] > nums[i-2] && nums[i-1] + nums[i-2] > nums[i] && nums[i-2] + nums[i] > nums[i-1]){
                return nums[i]+nums[i-1]+nums[i-2];
            }else{
                return 0;
            }
            
        } 
    }

int main() {
    vector<int> nums = {1,2,1,10};
    cout<<largestPerimeter(nums);


    return 0;
}
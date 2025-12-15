#include<bits/stdc++.h>
using namespace std;

bool check(vector<int>& nums){
    int count=0;
    for(int i=1;i<nums.size();i++){
        if(nums[i-1]>nums[i]){
            count++;
        }
    }
    if(nums[nums.size()-1]>nums[0]){
        count++;
    }

    return count<=1;

}

int main() {

    vector<int> nums;
   
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(3);
    nums.push_back(4);
    nums.push_back(5);
    nums.push_back(6);
    nums.push_back(7);
    nums.push_back(8);


    cout<<check(nums);
    
    return 0;
}
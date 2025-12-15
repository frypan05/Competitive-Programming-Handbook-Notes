#include <iostream>
#include<vector>
#include<cmath>
using namespace std;

//pivot and binary search combined

bool search(vector<int>& nums, int target) {


    int start = 0, end = nums.size() - 1;
    while(start <= end) {
        int mid = start + (end - start) / 2;
        if(nums[mid] == target) return true;
        if(nums[start] == nums[mid] && nums[mid] == nums[end]) {
              start ++;
              end --;
          }
        else if(nums[start] <= nums[mid]) {
            if(target >= nums[start] && target <= nums[mid]){
            end = mid - 1;}
            else{
            start = mid + 1;}
        }

        else {
            if(target <= nums[end] && target >= nums[mid])
            start = mid + 1;
            else
            end = mid - 1; 
    }
    }
    return false;
}

int main(){
    vector<int> nums = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1};
    int target = 2;

    bool found = search(nums, target);
    cout << (found ? "Found" : "Not Found") << endl;  
    
    return 0;
}

#include <iostream>
#include<vector>
#include<cmath>
using namespace std;


int getpivot(vector<int>& nums,int n){
    int s =0;
    int e = n-1;
    int mid = s+ (e-s)/2;
    while(s<e){
        if(nums[mid]>=nums[0]){
          s=mid+1;
    }else{
        e=mid;
        }
    mid=s + (e-s)/2;
    }
    return s;
}


int binarysearch(vector<int>& nums,int s,int e,int key){
    int start =s;
    int end =e;
    int mid = start + (end-start)/2;
    while(start<=end){
        if(nums[mid]==key){
            return mid;
        }
        if(key>nums[mid]){
            start=mid+1;
        }else{
            end =mid-1;
        }
        mid=start+(end-start)/2;
    }
    return -1;
}




int main() {
    std::vector<int> nums = {7,9,1,2,3};
    int n=nums.size();
    int target=2;
    int pivot =getpivot(nums,n);
    if(target>=nums[pivot]&& target<=nums[n-1]){
        return binarysearch(nums,pivot,n-1,target);
    }else{
        return binarysearch(nums,0,pivot-1,target);
    }
    
    return 0;
}

//check alternate methods

//Explanation
//
//
//Let's say nums looks like this: [12, 13, 14, 15, 16, 17, 18, 19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
//
//
//Because it's not fully sorted, we can't do normal binary search. But here comes the trick:
//
//
//If target is let's say 14, then we adjust nums to this, where "inf" means infinity:
//[12, 13, 14, 15, 16, 17, 18, 19, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf, inf]
//
//
//If target is let's say 7, then we adjust nums to this:
//[-inf, -inf, -inf, -inf, -inf, -inf, -inf, -inf, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
//
//
//And then we can simply do ordinary binary search.
//
//
//Of course we don't actually adjust the whole array but instead adjust only on the fly only the elements we look at. And the adjustment is done by comparing both the target and the actual element against nums[0].
//
//
//Code
//
//
//If nums[mid] and target are "on the same side" of nums[0], we just take nums[mid]. Otherwise we use -infinity or +infinity as needed.


int search(vector<int>& nums, int target) {
    int lo = 0, hi = nums.size();
    while (lo < hi) {
        int mid = (lo + hi) / 2;

//double num;
//
//if ((nums[mid] < nums[0]) == (target < nums[0])) {
//    num = nums[mid]; // Both are on the same side of nums[0]
//} else if (target < nums[0]) {
//    num = -INFINITY; // Target is less than nums[0]
//} else {
//    num = INFINITY;  // Target is greater than or equal to nums[0]
//}

        
        double num = (nums[mid] < nums[0]) == (target < nums[0])
                   ? nums[mid]
                   : target < nums[0] ? -INFINITY : INFINITY;
                   
        if (num < target)
            lo = mid + 1;
        else if (num > target)
            hi = mid;
        else
            return mid;
    }
    return -1;
}
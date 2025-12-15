#include<bits/stdc++.h>
using namespace std;

void rotate(vector<int>& nums, int k){
    vector<int> temp(nums.size());

    for(int i=0;i<nums.size();i++){
        temp[(i+k)%nums.size()] = nums[i];
    }

    //copy temp into num vector

    nums=temp;
    
}

//void rotate2(vector<int>& nums, int k) {
//    k = k%nums.size();
//    reverse(nums.begin(), nums.end());
//    cout<<"after first reverse"<<endl;
//    printarr(nums);
//    reverse(nums.begin(), nums.begin()+k);
//    cout<<"after second reverse"<<endl;
//    printarr(nums);
//    reverse(nums.begin()+k, nums.end());
//    cout<<"after third reverse"<<endl;
//    printarr(nums);
//}

//1. Reverse the whole vector
//2. Reverse the first k elements of the vector
//3. Reverse the remaining elements of the vector
//
//Dry run:
//
//arr = {1, 2, 3, 4}
//k = 2
//step 1: {4, 3, 2, 1}
//step 2: Reverse first two elements -> {3, 4, 2, 1} (k = 2 -> reverse elements present at 0th and 1st index)
//step 3: Reverse the remaining elements -> {3, 4, 1 , 2}





void printarr(vector<int> & nums){
        for(int i=0;i<nums.size();i++){
        cout<<nums[i]<< " ";
    }
}

int main() {

    vector<int> nums;
    int k = 3;
    nums.push_back(1);
    nums.push_back(2);
    nums.push_back(3);
    nums.push_back(4);
    nums.push_back(5);
    nums.push_back(6);
    nums.push_back(7);
    nums.push_back(8);

    //rotate(nums,k);
    //rotate2(nums,k);

    printarr(nums);

    return 0;
}
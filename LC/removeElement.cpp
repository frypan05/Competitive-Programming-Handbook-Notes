#include<bits/stdc++.h>
using namespace std;
int main(){
    int n;
    int val;
    cin >> n;
    cin >> val;

    vector <int> nums(n);
    for (int i = 0; i < n; i++) cin >> nums[i];
    // // [3,2,2,3], val = 3
    // // I1: k = 0; i = 0;  nums[k] = 3 -> skip
    // // I2: k = 0; i = 1;  nums[k] = 2 -> nums[0] = 2; [2,2,2,3]; k = 1
    // // I3: k = 1; i = 2;  nums[k] = 2 -> nums[1] = 2; [2,2,2,3]; k = 2
    // // I4: k = 2; i = 3;  nums[k] = 3 -> skip
    // int k = 0;
    // for (int i = 0; i < n; i++){
    //     if(nums[i] != val){
    //         nums[k] = nums[i];
    //         k++;
    //     }
    // }

    // cout << k << endl;

    // // Print the remaining elements
    // for (int i = 0; i < k; i++){
    //     cout << nums[i] << " ";
    // }

    // return k;


    //TWO POINTER APPROACH:
    int left = 0;
    int right = nums.size() - 1;

    while (left <= right){
        if (nums[left] == val){
            swap (nums[left], nums [right]);
            right--;
        }
        else{
            left++;
        }
    }
    return left;
}

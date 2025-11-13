#include<bits/stdc++.h>
using namespace std;

int main() {
    int arr[] = {1, 8, 3, 6, 8, 2};
    int n = sizeof(arr) / sizeof(arr[0]);
    
    int max_sum = arr[0], current_sum = arr[0];
    int start = 0, end = 0;
    int temp_start = 0;
    
    for (int i = 1; i < n; i++) {
        if (arr[i] > current_sum + arr[i]) {
            current_sum = arr[i];
            temp_start = i;
        } else {
            current_sum += arr[i];
        }
        
        if (current_sum > max_sum) {
            max_sum = current_sum;
            start = temp_start;
            end = i;
        }
    }
    cout << "Maximum subarray sum: " << max_sum << endl;
    cout << "Subarray: ";
    for (int i = start; i <= end; i++) {
        cout << arr[i] << " ";
    }
    return 0;
}

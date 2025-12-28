#include<bits/stdc++.h>
using namespace std;

void leftRotate(vector<int>& arr, int d) {
    int n = arr.size();
    d = d % n;
    
    reverse(arr.begin(), arr.begin() + d);

    reverse(arr.begin() + d, arr.end());

    reverse(arr.begin(), arr.end());
}

int main() {
    vector<int> arr = {5, 1, 8, 3, 4};
    int d = 4;
    
    leftRotate(arr, d);
    
    cout << "After left rotation by " << d << ": ";
    for(int num : arr) {
        cout << num << " ";
    }
    return 0;
}

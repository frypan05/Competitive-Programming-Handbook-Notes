#include <bits/stdc++.h>
using namespace std;

int main(){
    int n;
    cin >> n;
    vector <long long> a(n);
    for (int i = 0; i < n; i++ ) cin >> a[i];


    //initializes a pointer to the minimum element in the vector a
    long long mn = *min_element(a.begin(), a.end());
    cout << mn << "\n";
    // long long mn = a[0];
    // for (int i = 1; i < n; i++){
    //     mn = min(mn, a[i]);
    // }
    // cout << mn << "\n";
    return 0;
}

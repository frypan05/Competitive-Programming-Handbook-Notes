#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;

    while (t--) {
        int k, x;
        cin >> k >> x;

        // Minimal n such that no valid string exists
        cout << k * x + 1 << "\n";
    }
    return 0;
}

#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        vector<int> a(n);
        for (int i = 0; i < n; i++) cin >> a[i];

        long long total = 0;
        for (int i = 0; i + 1 < n; i++)
            total += llabs(a[i] - a[i + 1]);

        long long bestReduction = 0;
        for (int i = 1; i + 1 < n; i++) {
            long long reduction =
                llabs(a[i - 1] - a[i]) +
                llabs(a[i] - a[i + 1]) -
                llabs(a[i - 1] - a[i + 1]);
            bestReduction = max(bestReduction, reduction);
        }

        cout << total - bestReduction << "\n";
    }
    return 0;
}

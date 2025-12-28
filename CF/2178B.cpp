#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        string r;
        cin >> r;
        int n = r.size();
        for (char c : r) {
            assert(c == 's' || c == 'u');
        }
        int totalU = 0;
        for (char c : r) if (c == 'u') totalU++;
        int keptU = 0;
        int lastKept = -2;

        for (int i = 1; i <= n - 2; i++) {
            if (r[i] == 'u' && i - lastKept >= 2) {
                keptU++;
                lastKept = i;
            }
        }
        cout << totalU - keptU << "\n";
    }
    return 0;
}

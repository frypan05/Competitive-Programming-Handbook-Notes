#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        string s;
        cin >> s;

        bool hasN = false;
        for (char c : s) {
            if (c == 'N') {
                hasN = true;
                break;
            }
        }

        cout << (hasN ? "YES\n" : "NO\n");
    }
    return 0;
}

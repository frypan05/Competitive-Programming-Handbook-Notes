#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int t;
    cin >> t;
    while (t--) {
        long long n;
        cin >> n;
        long long temp = n;
        long long k = 1;
        for (long long i = 2; i * i <= temp; i++) {
            if (temp % i == 0) {
                k *= i;
                while (temp % i == 0)
                    temp /= i;
            }
        }
        if (temp > 1)
            k *= temp;
        cout << k << "\n";
    }
    return 0;
}

// C++ program to find nth catalan number

#include <iostream>
using namespace std;

int findCatalan(int n) {
    // Base case
    if (n <= 1)
        return 1;

    // catalan(n) is sum of
    // catalan(i)*catalan(n-i-1)
    int res = 0;
    for (int i = 0; i < n; i++)
        res += findCatalan(i) * findCatalan(n - i - 1);

    return res;
}

int main() {
    int n = 6;
    int res = findCatalan(n);
    cout << res;
    return 0;
}
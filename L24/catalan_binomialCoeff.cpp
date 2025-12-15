// C++ program for nth Catalan Number

#include <iostream>
using namespace std;

// Returns value of Binomial Coefficient C(n, k)
int binomialCoeff(int n, int k) {
    int res = 1;

    // Since C(n, k) = C(n, n-k)
    if (k > n - k)
        k = n - k;

    // Calculate value of [n*(n-1)*---*(n-k+1)] /
    // [k*(k-1)*---*1]
    for (int i = 0; i < k; ++i)
    {
        res *= (n - i);
        res /= (i + 1);
    }

    return res;
}

// A Binomial coefficient based function to find nth catalan
// number in O(n) time
int findCatalan(int n) {
  
    // Calculate value of 2nCn
    int c = binomialCoeff(2 * n, n);

    // return 2nCn/(n+1)
    return c / (n + 1);
}

int main() {
    int n = 6;
    int res = findCatalan(n);
    cout << res;
    return 0;
}


// Time Complexity: O(n).
// Auxiliary Space: O(1)
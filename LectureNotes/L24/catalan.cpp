//nth Catalan Number using the (n-1)th Catalan Number:

#include <iostream>
using namespace std;

int findCatalan(int n) {
    int res = 1;

    // Use the iterative approach to 
  // calculate the nth Catalan number
    for (int i = 2; i <= n; i++) {

        res = ((res * (4 * i - 2)) / (i + 1));
    }

    return res;
}

int main() {
    int n = 6;
    int res = findCatalan(n);
    cout << res;
    return 0;
}

//Time Complexity: O(n)
//Auxiliary Space: O(1),
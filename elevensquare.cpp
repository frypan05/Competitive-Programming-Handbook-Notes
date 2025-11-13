#include<bits/stdc++.h>
using namespace std;

int main() {
    int n = 5;
    for (int i = 0; i < n; i++) {
      long long powerof11 = static_cast<long long>(pow(11, i));
      cout << powerof11 << endl;
    }
    return 0;
}

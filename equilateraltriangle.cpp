#include<bits/stdc++.h>
using namespace std;

int main() {
    int n = 5;
    int val;

    for (int i = 0; i <= n; i++) {
      val = 1;
      for (int j = 0; j <= i; j++) {
        cout << val;
        val = val * (i-j)/(j+1);
      }
      cout << " " <<endl;
      
    }
    return 0;
}


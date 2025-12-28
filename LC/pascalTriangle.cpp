#include <bits/stdc++.h>
using namespace std;

void pascalTriangle (int n){
  long long ans = 1;
  cout << ans << " ";

  for (int i = 1; i < n; i++) {
    ans = ans*(n-i);
    ans = ans/i;
    cout << ans << " ";
  }
  cout << endl;
}

int main(){
  int n=5;
  pascalTriangle(n);
  return 0;
}

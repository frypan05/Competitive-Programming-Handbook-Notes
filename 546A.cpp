#include <bits/stdc++.h>
using namespace std;

int main () {
  int k,n,w,x,y;
  count = 0;

  cin >> k >> n >> w;
  for (int i = 1; i < w; i++) {
    x= k*i;
    count += x;
  }
  y = count - n;
  cout << y;
  
  return 0;
}

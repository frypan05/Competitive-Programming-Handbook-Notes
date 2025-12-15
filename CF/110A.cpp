#include <bits/stdc++.h>
using namespace std;

int main () {
  long long a;
  cin >> a;
  int count = 0;
  
  while (a>0) {
    int i = a%10;
    a/=10;
    if (i==4 || i==7) {
      count++;
    }
  }
  if (count == 4 || count == 7) {
    cout << "YES";  
  }
  else {
   cout << "NO";
  }
  return 0;
}

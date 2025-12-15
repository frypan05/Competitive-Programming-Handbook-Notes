#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

int main () {
  int tt;
  cin >> tt;
  while (tt--) {
    string a,b;
    cin >> a >> b;
    swap(a[0],b[0]);
    cout << a << ' ' << b << endl;
  }
  return 0;
}

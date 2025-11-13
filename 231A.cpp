#include <bits/stdc++.h>
using namespace std;

int main () {

  int x;
  cin >> x;
  int count = 0;

  for (int i=0; i < x ; i++){
    int a, b, c;

    cin >> a >> b >> c;
    if( a + b + c >= 2){
      count++;
    }
  }
  cout << count << endl;
  return 0;
}

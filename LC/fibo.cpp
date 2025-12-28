#include <bits/stdc++.h>
using namespace std;
int main(){
  int n;
  int a = 0;
  int b = 1;
  for (int i = 1; i <=n; i++){
    cout << a << " ";
    int x = a+b;
    int a = b;
    int b = x;
  }
  return 0;
}

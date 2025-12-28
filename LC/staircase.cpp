#include<bits/stdc++.h>
using namespace std;

int countWays(int n){
  if (n==0) return 1;
  if (n < 0 ) return 0;

  return (countWays(n-1)+countWays(n-2));
}

int main(){
  int steps;
  cin >> steps;
  cout << "Number of steps: " << steps << "Number of ways: " << countWays(steps) << endl;
  return 0;
}

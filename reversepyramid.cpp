#include<bits/stdc++.h>
using namespace std;

void pattern1(int n){
  for (int i = n; i >0; i--) {
    for (int sp = 0; sp< n-1; sp++) {
      cout << " " << (n-sp);
    }
    for (int j = 0; j < 2*i-1; j++) {
      cout << " " << i;
    }
    for (int sp = 0; sp < n-1; sp++) {
      cout << " " << (n-sp);
    }
    cout << "" << endl;
  }
}

void pattern180(int n){
  for (int i = 2; i < n; i++) {
    for (int sp = 0; sp < n){

    }
    
  }
}

int main() {
    int n = 4;
    cout << pattern1 << endl;
    return 0;
}


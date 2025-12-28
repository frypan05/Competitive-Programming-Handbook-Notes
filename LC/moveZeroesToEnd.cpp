#include<bits/stdc++.h>
using namespace std;
//bruteForce

vector <int> moveZeroes(int n, vector<int> a){
  vector<int> temp;

  for (int i = 0; i < n; i++) {
    if (a[i]!=0) {
      temp.push_back(a[i]);
    }
  }

  int nz= temp.size();
  for (int i = 0; i < nz; i++) {
    a[i] = temp[i];
  }
  for (int i = nz; i < n; i++) {
    a[i] = 0;
  }

  return a;
}


int main() {
    vector<int> arr = {1,0,2,3,2,0,0,4,5,1};
    int n = 10;
    vector<int> ans = moveZeroes(n,arr);
    for (auto &it : ans){
      cout << it << " ";
    }
    cout << "\n";
    return 0;
}


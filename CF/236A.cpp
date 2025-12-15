#include <bits/stdc++.h>
using namespace std;

int main () {
  string username;
  cin >> username;

  unordered_set<char> distinctLetters;
  for (char ch:username){
    distinctLetters.insert(ch);
  }

  if(distinctLetters.size() % 2 == 0){
    cout << "CHAT WITH HER" << endl;
  } else {
    cout << "IGNORE HIM" << endl;
  }
  
  return 0;
}

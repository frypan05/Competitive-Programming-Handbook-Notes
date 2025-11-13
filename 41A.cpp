#include <bits/stdc++.h>
using namespace std;
// USING TWO POINTER APPROACH
void reverseString(string &s){
  int left = 0, right = s.size() - 1;
  while (left < right) {
    swap(s[left], s[right]);
    left++;
    right--;
  }
}

int main () {
  string s;
  string t;

  cin >> s;
  cin >> t;
  /*
     USING REVERSE FUNCTION

  reverse(s.begin(), s.end());
  if (s==t) {
    cout << "YES";
  }
  else {
    cout << "NO";
  }
  */
  /*
     USING MANUAL METHOD LOOP
  for (int i = s.size()-1; i >= 0; i--) {
    rev += s[i];
  }
  if (s==t){
    cout << "YES";
  }
  else {
    cout << "NO";
  }
  */
  reverseString(s);
  if (s==t) {
    cout << "YES";
  }
  else {
    cout << "NO";
  }
  return 0;
}








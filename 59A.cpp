#include <bits/stdc++.h>
using namespace std;

int main () {

  string s;
  cin >> s;

  int lower_count = 0, upper_count = 0;

  for(char c : s){
    if (islower(c)) {
        lower_count++;
    } else {
      upper_count++;
    }
  }

  if (lower_count >= upper_count) {
    for (char &c : s) {
      c = tolower(c);
    }
    
  } else {
    for ( char &c : s) {
      c = toupper(c);
    }
  }
  cout << s << endl;

  return 0;
}

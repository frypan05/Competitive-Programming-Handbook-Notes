#include<bits/stdc++.h>
using namespace std;

int decToBin (int a){
  if (a = 0) return;
  decToBin(a/2); 
  cout << n%2;
}

int main() {
    int a;
    cout << "Enter No: ";
    cin >> a;
    cout << decToBin(a) << endl;
    return 0;
}


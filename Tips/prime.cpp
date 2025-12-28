#include <bits/stdc++.h>
using namespace std;

bool isPrime(int n){
  if (n <=1) return false;
  for (int i = 2; i <= sqrt(n);i++){
    if (n % i == 0) return false;
  }
  return true;
}


int main(){
  int start , end;
  cout << "Enter starting no."<<endl;
  cout << "Enter Ending no."<< endl;
  cin >> start >> end;
  for (int i = start; i <= end; i++){
    if(isPrime(i)){
      cout << i << " ";
    }
  }
  cout << endl;


  return 0;
}

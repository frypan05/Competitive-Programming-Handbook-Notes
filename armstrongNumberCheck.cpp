#include<bits/stdc++.h>
using namespace std;

int isArmstrong(int num){
  int original = num;
  int sum = 0;
  int digits = 0;
  int temp = num;

  while (temp > 0){
    digits++;
    temp /= 10;
  }

  temp = num;
  while (temp > 0){
    int digit = temp % 10;
    sum += pow(digit, digits);
    temp /= 10;
  }

  return original == sum;
}

int main (){
  int num;
  cin >> num;
  cout << "Is Armstrong?:  ";
  if (isArmstrong(num)){
    cout << "yes" << endl;
  }
  else{
    cout << "no" << endl;
  }
  return 0;
}



//Bubble Sort O(n^2)

#include <bits/stdc++.h>
using namespace std;
int main () {
  int array[] = {3,5,1,2,7,9};
  int n = sizeof(array) / sizeof(array[0]);  
   for ( i = 0; i < n; i++) {
    for ( j = 0; j < n-1; j++) {
      if (array[j] > array[j+1]) {
        swap(array[j], array[j+1]);
      }
    }
  }
  cout << "array";
  return 0;
}  

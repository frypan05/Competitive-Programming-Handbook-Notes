#include<bits/stdc++.h>
using namespace std;
vector<int> twosum( vector<int> & arr, int target){
  int n = arr.size();
  vector<pair<int,int>> index;

  for(int i = 0; i < n; i++){
    index.push_back({arr[i], i});
  }
  sort(index.begin(), index.end());
  int left = 0, right = n - 1;

  while(left < right){
    int sum = index[left].first + index[right].first;

    if(sum == target){
      return {index[left].second, index[right].second};
    }
    else if(sum > target){
      right--;
    }
    else{
      left++;
    }
  }
  return {};
}

int main() {
    int target = 110;
    vector <int> arr = {1,3,54,32,9,8,56};
    vector<int> result = twosum(arr, target);

    if(!result.empty()){
      cout << result[0] << result[1];
    }
    else{
      cout << "no twosum";
    }
    return 0;
}


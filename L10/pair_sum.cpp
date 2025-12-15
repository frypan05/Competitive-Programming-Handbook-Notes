//https://www.naukri.com/code360/problems/pair-sum_697295?source=youtube&campaign=love_babbar_codestudio1

#include<bits/stdc++.h>
using namespace std;

vector<vector<int>> pairSum(vector<int>& arr,int size,int s){

    vector<vector<int>> ans;
    

    for(int i=0;i<arr.size();i++){
        for(int j=i+1;j<arr.size()-1;j++){
            if((arr[i]+arr[j])==s){
                vector<int> temp;
                temp.push_back(min(arr[i],arr[j]));
                temp.push_back(max(arr[i],arr[j]));
                ans.push_back(temp);
            }
        }
    }

    sort(ans.begin(),ans.end());
    return ans;
}

int main() {

    vector<int> arr;
    arr.push_back(1);
    arr.push_back(2);
    arr.push_back(3);
    arr.push_back(4);
    arr.push_back(5);
    int size=5;
    int s = 5;
    

    vector<vector<int>> ans( pairSum(arr,size,s));

    for(int i=0;i<2;i++){
        for(int j =0;j<2;j++){
            cout<<ans[i][j]<<" ";
        }cout<<endl;
    }
        

    
    return 0;
}
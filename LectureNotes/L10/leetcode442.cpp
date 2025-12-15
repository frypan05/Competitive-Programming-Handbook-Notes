#include<bits/stdc++.h>
using namespace std;

vector<int> allDuplicates(vector<int> nums){
    unordered_map<int,int> map;
    vector<int> dupes;
    for(auto i:nums){
        map[i]++;
    }
    for(auto it:map){
        int freq = it.second;
        if(freq!=1){
        dupes.push_back(it.first);
        }   
    }
    return dupes;
        

}

int main() {

    vector<int> nums={1,2,2,3,4,4,5,5,6,7,8,9};
    
    allDuplicates(nums);
    for(int i=0;i<allDuplicates(nums).size();i++){
        cout<<allDuplicates(nums)[i];
    }
    
    return 0;
}
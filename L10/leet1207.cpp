#include <bits/stdc++.h>
using namespace std;

//2 set approach

bool uniqueOccurences(vector<int> arr){
    
    sort(arr.begin(),arr.end());
    unordered_set<int> element;
    unordered_set<int> occur;
    for(int i=0;i<arr.size();i++){
        element.insert(arr[i]);
    }
    int ac=1;
    for(int i=0;i<arr.size()-1;i++){
        if(arr[i]==arr[i+1]){
            ac++;
        }
        if(arr[i]!=arr[i+1]){
            occur.insert(ac);
            ac=1;
        }
    }
    occur.insert(ac);
    if(element.size()==occur.size()){
        return true;
    }
    return false;

}

//map and set approach
bool uniqueOccurence(vector<int> arr){
    unordered_map<int,int> map;
    unordered_set<int> set;
    for(auto it:arr){
        map[it]++;
    }

    for(auto it:map){
        int freq=it.second;
        set.insert(freq);
    
    }
    return map.size()==set.size();
}



int main() {

    vector<int> arr={1,2,2,3,3,4,4,4,4};
    cout<<uniqueOccurences(arr)<<"\n";
    cout<<uniqueOccurence(arr);
    

    return 0;
}

#include<bits/stdc++.h>
using namespace std;

//Approach 1 → Hashmap
vector<string> removeAnagrams(vector<string>& words) {
    vector<unordered_map<char,int>> freq(words.size());

    for(int i =0;i<words.size();i++){
        for(char ch :words[i]){
            freq[i][ch]++;
        }
    }

    vector<string> ans;
    ans.push_back(words[0]);
    for(int i =1;i<words.size();i++){
        if(freq[i]!=freq[i-1]){
            ans.push_back(words[i]);
        }
    }
    return ans;
}

//Approach 2 → Sorting
vector<string> removeAnagrams(vector<string>& words) {
    vector<string> ans;
    ans.push_back(words[0]);

    for(int i=1;i<words.size();i++){
        string a = words[i];
        string b= ans.back();

        sort(a.begin(),a.end());
        sort(b.begin(),b.end());
        if(a!=b){
            ans.push_back(words[i]);
        }
    }
    return ans;
}




int main() {
    
    return 0;
}
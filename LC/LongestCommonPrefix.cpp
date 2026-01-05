#include<bits/stdc++.h>
using namespace std;

int main (){
    string LongestCommonPrefix(vector<string>& strs){
        if (strs.empty()) return "";

        for (int i =0 ; i <strs[0].size(); i++){
            char c = strs[0][i];

        for (int j = 1; j < strs.size(); j++){
            if (i == strs[j].size() || strs [j][i] != c){
                return strs[0].substr(0,1);

            }
        }
        return strs[0];
        }
    }

}

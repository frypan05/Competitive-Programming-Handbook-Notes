#include<bits/stdc++.h>
using namespace std;

void solve (string var,vector<string>& ans, int index){
    //base case
    if(index>=var.size()){
        ans.push_back(var);
        return;
    }

    for(int j = index; j<var.size();j++){
        swap(var[index],var[j]);
        solve(var,ans,index+1);
        //backtrack
        //swap(var[index],var[j]); //not needed
    }
}

vector<string> permute(string& var){
    vector<string> ans;
    int index =0;
    solve(var, ans, index);
    return ans;
}

int main() {

    string var = "abc";

    vector<string> result = permute(var);

    for(int i = 0; i < result.size(); i++){
        cout << result[i] << endl; 
    }
    
    return 0;
}
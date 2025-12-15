#include<bits/stdc++.h>
using namespace std;

void solve(string digits,string output, int index, vector<string>& ans,string mapping[]){
    
    //base case
    if(index>=digits.length()){
        ans.push_back(output);
        return;
    }

    //digits->23 //i at 2
    int number = digits[index]-'0'; //char to int conversion //2
    string value = mapping[number]; //abc

    for(int i=0;i<value.length();i++){
        output.push_back(value[i]);
        solve(digits,output,index+1,ans ,mapping);
        output.pop_back(); //char mapped to first digit removed //backtracked //will execute after each recursion call to remove individual chars

        //w/o pop_back -> ad ade adef abd abde abdef abcd abcde abcdef //23
        //with pop_back -> ad ae af bd be bf cd ce cf //23
    }
}

vector<string> letterCombinations(string digits) {
    
    vector<string> ans;
    if(digits.length()==0){
        return ans;
    }
    string output;
    int index = 0;

    string mapping[10]={"","","abc","def","ghi","jkl","mno","pqrs","tuv","wxyz"};
    solve(digits,output,index,ans,mapping);
    return ans;
}

int main() {

    string digits = "89";

    vector<string> ans = letterCombinations(digits);

    for(int i =0;i<ans.size();i++){
        cout<<ans[i]<<" ";
    }
     

    
    return 0;
}
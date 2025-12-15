//subsets - https://leetcode.com/problems/subsets/description/

#include<bits/stdc++.h>
using namespace std;

// void solve(vector<string> str, vector<string> output, int index, vector<vector<string>>& ans){

//     //base case
//     if(index>=str.size()){
//         ans.push_back(output);
//         return;
//     }

//     //exclude
//     solve(str,output,index+1,ans);

//     //include
//     string element = str[index];
//     output.push_back(element);
//     solve(str,output,index+1,ans);
// }

// vector<vector<string>> subsets(vector<string>& str){
    
//     vector<vector<string>> ans;
//     vector<string> output;
//     int index =0;
//     solve(str,output, index,ans);

//     return ans;

// }

// int main() {

//     vector<string> str;
//     str.push_back("a");
//     str.push_back("b");
//     str.push_back("c");
//     str.push_back("d");

//     vector<vector<string>> fin_ans = subsets(str);

//     for (int i = 0; i < fin_ans.size(); i++) {
//         cout<<'[';
//         for (int j = 0; j < fin_ans[i].size(); j++) { 
//             cout << fin_ans[i][j]; 
//         } 
//         cout<<"] ";
//     }
 
//     return 0;
// }

void solve(string str, string output, int index, vector<string>& ans){

    //base case
    if(index>=str.length()){
        if(output.length()>0){ //removes empty string
            ans.push_back(output);
        }
       
        return;
    }

    //exclude
    solve(str,output,index+1,ans);

    //include
    char element = str[index];
    output.push_back(element);
    solve(str,output,index+1,ans);
}

vector<string> subsets(string str){
    
    vector<string> ans;
    string output = "";
    int index = 0;
    solve(str,output, index,ans);

    return ans;

}


int main() {

    string str ="abc";
    vector<string> fin_ans = subsets(str);

    for (int i = 0; i < fin_ans.size(); i++) {
        cout<<"["<< fin_ans[i]<< "]"; 
    }
 
    return 0;
}


//HW -bit manipulation
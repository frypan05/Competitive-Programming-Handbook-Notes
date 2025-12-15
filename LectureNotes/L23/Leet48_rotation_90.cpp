#include<bits/stdc++.h>
using namespace std;


void rotate(vector<vector<int>>& matrix) {
    
    int len = matrix.size();
    

    //taking transpose 
    for(int i=0;i<len;i++){
        for(int j=i+1;j<len;j++){
            
            swap(matrix[i][j],matrix[j][i]);
            
        }
    }

    //reversing 

    for(int i =0;i<len;i++){
        reverse(matrix[i].begin(),matrix[i].end());
    }
        
}

int main() {

    vector<vector<int>> matrix;

    matrix.push_back({1,2,3});
    matrix.push_back({4,5,6});
    matrix.push_back({7,8,9});
    matrix.push_back({7,8,9});

    rotate(matrix);

    for( int row = 0 ;row<3;row++){
        for(int col=0;col<3;col++){
            cout<<matrix[row][col]<<" ";
        }cout<<endl;
    }
    
    // cout<<matrix.size(); //length of rows
    // cout<<matrix[0].size(); //lengthn of cols
    return 0;
}
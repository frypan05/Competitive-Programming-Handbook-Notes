#include<bits/stdc++.h>
using namespace std;

bool searchMatrix(vector<vector<int>>& matrix, int target) {
    
    int row = matrix.size();
    int col = matrix[0].size();    

    int rowIndex=0;
    int colIndex=col-1;

    while(rowIndex<row && colIndex>=0){ 

        int element = matrix[rowIndex][colIndex];

        if(target==element){
            return 1;
        } 

        if(target>element){
            rowIndex++;
        }
        if(target<element){
            colIndex--;
        } 
    }
    return 0;
}


int main() {
    
    vector<vector<int>> matrix;

    matrix.push_back({1,4,7,11,15});
    matrix.push_back({2,5,8,12,19});
    matrix.push_back({3,6,9,16,22});
    matrix.push_back({10,13,14,17,24});
    matrix.push_back({18,21,23,26,30});

    int target=21;

    if(searchMatrix(matrix,target)){
        cout<<"element is present";
    }else{
        cout<<"element is not present";
    }


    return 0;
}
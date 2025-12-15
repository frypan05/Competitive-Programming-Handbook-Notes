#include<bits/stdc++.h>
using namespace std;

bool searchMatrix(vector<vector<int>>& matrix, int target) {

    int row = matrix.size();
    int col = matrix[0].size();    

    int s=0;
    int e=row*col-1;
  
    int mid = s + (e-s)/2;

    while(s<=e){

        int element = matrix[mid/col][mid%col];

        if(target==element){
            return 1;
        } 

        if(target>element){
            s=mid+1;
        }
        if(target<element){
            e=mid-1;
        }
        mid = s + (e-s)/2;
    }
    return 0;

}    

int main() {

    vector<vector<int>> matrix;

    matrix.push_back({1,2,3});
    matrix.push_back({4,5,6});
    matrix.push_back({7,8,9});

    int target=50;

    if(searchMatrix(matrix,target)){
        cout<<"element is present";
    }else{
        cout<<"element is not present";
    }
    
    
    return 0;
}
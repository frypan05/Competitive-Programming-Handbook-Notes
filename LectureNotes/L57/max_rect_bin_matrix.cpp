#include<bits/stdc++.h>
using namespace std;


vector<int> prevSmallerElement(vector<int> &arr, int n)
{
    stack<int> s;
    s.push(-1);
    vector<int> ans(n);

    for(int i =0;i<n;i++){
        int curr = arr[i];
        while(s.top()!=-1 &&  arr[s.top()]>=curr){ // ans is not stack ka top
            s.pop();
        }
        //ans is stack ka top
        ans[i]=s.top();
        s.push(i);
    }
    return ans;
}

vector<int> nextSmallerElement(vector<int> &arr, int n)
{
    stack<int> s;
    s.push(-1);
    vector<int> ans(n);

    for(int i =n-1;i>=0;i--){
        int curr = arr[i];
        while(s.top()!=-1 && arr[s.top()]>=curr){ // ans is not stack ka top
            s.pop();
        }
        //ans is stack ka top
        ans[i]=s.top();
        s.push(i);//modified due need of indexes
    }
    return ans;
}


int largestRectangleArea(vector<int>& heights) {
    int n = heights.size();

    vector<int> next(n);
    next = nextSmallerElement(heights,n);

    vector<int> prev(n);
    prev = prevSmallerElement(heights,n);

    int area = INT_MIN;

    for(int i =0;i<n;i++){
        int l = heights[i];
    
        if(next[i]==-1){
            next[i]=n;
        }
        int b = next[i]-prev[i]-1;

        int newArea = l*b;
        area = max(area,newArea);

    }
    return area;

}


int maxArea(vector<vector<int>> &mat) {
    //empty matrix
    if (mat.empty() || mat[0].empty()) return 0;

    int n= mat.size();
    int m = mat[0].size();


    //initialise histogram with 1st row
    vector<int> heights(m,0); //col length
    int area = 0;

    for(int i =0;i<n;i++){
        for(int j=0;j<m;j++){

            //row update by adding previous row's value
            if(mat[i][j]!=0){
                heights[j] +=1; //height increment when 1
            }else{
                heights[j] = 0; //reset height to 0  when 0 encountered
            }
        }
        //entire row is updated now
        area = max(area , largestRectangleArea(heights));
    }
    return area;
}

int main() {
    
    return 0;
}
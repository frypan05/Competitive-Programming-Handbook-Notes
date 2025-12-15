//https://www.geeksforgeeks.org/problems/rat-in-a-maze-problem/1

#include<bits/stdc++.h>
using namespace std;

bool isSafe(int x, int y,int n, vector<vector<int>>& visited,vector<vector<int>>& maze){
    if((x>=0 && x<n) && (y>=0 && y<n) && visited[x][y]==0 && maze[x][y]==1){
        return true;
    }
    return false;
}

void solve(vector<vector<int>>& maze, int n,vector<string>& ans,int x, int y, vector<vector<int>>& visited,string path){
    //reached x,y here
    //base case
    if(x==n-1 && y==n-1){
        ans.push_back(path);
        return;
    }
    visited[x][y]=1;
    
    //4 choices - D,L,R,U

    //down
    int newx = x+1;
    int newy = y;
    if(isSafe(newx,newy,n,visited,maze)){
        path.push_back('D');
        solve(maze,n,ans,newx,newy,visited,path);
        path.pop_back(); //backtrack 
    }

    //up
    newx = x-1;
    newy = y;
    if(isSafe(newx,newy,n,visited,maze)){
        path.push_back('U');
        solve(maze,n,ans,newx,newy,visited,path);
        path.pop_back(); 
    }

    //left
    newx = x;
    newy = y-1;
    if(isSafe(newx,newy,n,visited,maze)){
        path.push_back('L');
        solve(maze,n,ans,newx,newy,visited,path);
        path.pop_back(); 
    }

    //right
    newx = x;
    newy = y+1;
    if(isSafe(newx,newy,n,visited,maze)){
        path.push_back('R');
        solve(maze,n,ans,newx,newy,visited,path);
        path.pop_back(); 
    }

    visited[x][y]=0;
}

vector<string> ratInMaze(vector<vector<int>>& maze,int n) {
    vector<string> ans;

    //base case
    if(maze[0][0]==0){
        return ans;
    }

    int srcx =0;
    int srcy =0;

    vector<vector<int>> visited = maze;

    //copied array initialized with 0
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            visited[i][j]=0;
        }
    }

    string path="";
    solve(maze,n,ans,srcx,srcy,visited,path);
    sort(ans.begin(),ans.end());
    return ans;
}

int main() {

    vector<vector<int>> maze;
    maze.push_back({1,0,0,0}); 
    maze.push_back({1,1,0,1});
    maze.push_back({1,1,0,0});
    maze.push_back({0,1,1,1});

    int n=maze.size();
    vector<string> result = ratInMaze(maze,n);

    cout << "Paths found:" << endl;
    for (const string& path : result) {
        cout << path << endl;
    }
    
    return 0;
}
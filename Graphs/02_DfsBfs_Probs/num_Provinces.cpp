//https://www.geeksforgeeks.org/problems/number-of-provinces/1

#include<bits/stdc++.h>
using namespace std;

void dfs(int node, vector<int> adjList[], int vis[]){
    vis[node]=1;
    
    //traverse all neighbours
    for(auto it: adjList[node]){
        if(!vis[it]){
            dfs(it,adjList,vis);
        } 
    }
}



int numProvinces(vector<vector<int>> adj, int V) {
    
    //adjacency matrix to list
    vector<int> adjList[V];
    for(int i = 0;i<V;i++){
        for(int j =0;j<V;j++){
            if (adj[i][j]==1 && i!=j){
                adjList[i].push_back(j);
                adjList[j].push_back(i);
            }
        }
    }

    int vis[V] = {0};
    int cnt=0;
    for(int i = 0 ;i<V;i++){
        if(!vis[i]){
            cnt++;
            dfs(i,adjList,vis);
        }
    }
    return cnt;
}

int main() {
    
    return 0;
} 
#include<bits/stdc++.h>
using namespace std;


void dfs(int node, vector<int> adj[], int vis[], vector<int> &ls){
    vis[node]=1;
    ls.push_back(node);
    
    //traverse all neighbours
    for(auto it: adj[node]){
        if(!vis[it]){
            dfs(it,adj,vis,ls);
        }
    }
}

//0 based indexing
vector<int> dfsOfGraph(int V , vector<int> adj[]){
    int vis[V]={0};
    int start =0;
    vector<int> ls;
    dfs(start,adj,vis,ls);
    return ls;
}

int main() {
    
    return 0;
}
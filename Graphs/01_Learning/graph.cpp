#include<bits/stdc++.h>
using namespace std;

//matrix method to store undirected graph
int main() {
    int n,m;
    cin>>n>>m;

    //graph here
    int adj[n+1][m+1];
    for(int i =0;i<m;i++){
        int u,v;
        cin>> u>>v;
        adj[u][v]=1; // when unit weights are considered
        adj[v][u]=1;

        //adj[u][v]=wt; // when varying weights are considered
        //adj[v][u]=wt;
    }
    
    return 0;
}

//list method to store undirected graph
int main() {
    int n,m;
    cin>>n>>m;

    //graph here
    vector<int> adj[n+1];
    for(int i =0;i<m;i++){
        int u,v;
        cin>> u>>v;
        adj[u].push_back(v); //on the uth index, store v as v is a neighbour
        adj[v].push_back(u); //on the vth index, store u as u is a neighbour
    }
    
    return 0;
}

//list method to store directed graph
int main() {
    int n,m;
    cin>>n>>m;

    //graph here
    vector<int> adj[n+1];
    for(int i =0;i<m;i++){
        int u,v;
        // u ----> v
        cin>> u>>v;
        adj[u].push_back(v); 
    }
    
    return 0;
}
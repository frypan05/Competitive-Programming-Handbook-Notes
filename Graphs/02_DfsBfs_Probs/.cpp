#include <bits/stdc++.h>
using namespace std;

long long dfs(int u, int p, const vector<vector<int>>& adj, const vector<int>& costs, vector<long long>& subtreeSum) {
    // Initialize sum with the current node's cost
    // Note: Node u is 1-based, but costs vector is 0-based.
    long long currentSum = costs[u - 1];
    
    for (int v : adj[u]) {
        if (v != p) {
            currentSum += dfs(v, u, adj, costs, subtreeSum);
        }
    }
    
    // Store and return the sum for this node
    return subtreeSum[u] = currentSum;
}

int minimumInflation(int N, int K, int U[], int V[], int C[]) {
    // 1. Build the Graph (Adjacency List)
    // Using N + 1 size for 1-based indexing of cities
    vector<vector<int>> adj(N + 1);
    
    for (int i = 0; i < N - 1; ++i) {
        adj[U[i]].push_back(V[i]);
        adj[V[i]].push_back(U[i]);
    }

    // 2. Prepare data structures
    vector<long long> subtreeSum(N + 1, 0);
    vector<int> costs(C, C + N); // Copy C array to vector for easier access

    // 3. Run DFS from the root (City 1) to calculate subtree sums
    dfs(1, -1, adj, costs, subtreeSum);

    // 4. Calculate X values for all cities
    vector<long long> xValues;
    for (int i = 1; i <= N; ++i) {
        // Formula derived from problem: 
        // X = (K * Cost) + (SubtreeSum - Cost)
        // X = SubtreeSum + (K - 1) * Cost
        long long val = subtreeSum[i] + (long long)(K - 1) * costs[i - 1];
        xValues.push_back(val);
    }

    // 5. Sort to find the minimum difference between any two cities
    sort(xValues.begin(), xValues.end());

    // 6. Find minimum absolute difference
    long long minDiff = -1;
    
    if (N >= 2) {
        // Initialize with the difference between the first two elements
        minDiff = xValues[1] - xValues[0];
        
        // Scan the rest of the sorted array
        for (size_t i = 1; i < xValues.size() - 1; ++i) {
            long long diff = xValues[i+1] - xValues[i];
            if (diff < minDiff) {
                minDiff = diff;
            }
        }
    } else {
        return 0; // Fallback for single node (though constraints say N>=2)
    }

    return (int)minDiff;
}

// ---------------------------------------------------------
// Driver Code (Optional: For testing locally, do not copy to the online editor if they provide main)
int main() {
    // Sample Input Test
    int N = 8;
    int K = 2;
    int U[] = {1, 1, 6, 6, 2, 2, 2};
    int V[] = {4, 6, 2, 3, 5, 7, 8};
    int C[] = {3, 5, 8, 10, 2, 6, 7, 11};

    cout << minimumInflation(N, K, U, V, C) << endl; // Expected Output: 2

    return 0;
}
#include<bits/stdc++.h>
using namespace std;
//https://gemini.google.com/app/bbf5e75ff8f7a96b?hl=en-IN
// Example: If energy = [5, 2, -1, 3, 4] and k=2:

// Start at index 0: 5+(−1)+4=8 (Jumps: 0→2→4)
// Start at index 1: 2+3=5 (Jumps: 1→3)
// Start at index 2: −1+4=3 (Jumps: 2→4)
// Start at index 3: 3 (Jump: 3→5 (out of bounds))
// Start at index 4: 4 (Jump: 4→6 (out of bounds))
    
// The maximum energy is 8.

//approach 1
int maximumEnergy(vector<int>& energy, int k) {
    int n = energy.size();
    vector<int> dp(n);
    int MaxEnergy = INT_MIN;
    for (int i = n - 1; i >= 0; --i) {
        // dp[i] = energy[i] + ((i + k < n) ? dp[i + k] : 0);
        if(i+k<n) dp[i]=energy[i]+ dp[i+k];
        else dp[i]=energy[i];
        MaxEnergy = max(MaxEnergy, dp[i]);
    }
    return MaxEnergy;
}

//approach2
int maximumEnergy(vector<int>& energy, int k) {
    int n = energy.size();
    vector<int> dp(n);
    int MaxEnergy = INT_MIN;
    for (int i = n - 1; i >= 0; --i) {
        
        if(i+k<n) dp[i]=energy[i]+ dp[i+k];
        else dp[i]=energy[i];

        if(dp[i]>MaxEnergy){
            MaxEnergy=dp[i];
        }
    }
    return MaxEnergy;
}



int main() {
    vector<int> energy = {5, 2, -1, 3, 4};
    int k = 2;

    cout<<maximumEnergy(energy,k);
    
    return 0;
}
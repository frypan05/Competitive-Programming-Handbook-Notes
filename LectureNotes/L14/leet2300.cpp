#include<bits/stdc++.h>
using namespace std;

vector<int> successfulPairs(vector<int>& spells, vector<int>& potions, long long success) {
    
    
    vector<int> ans;

    for(int i= 0;i<spells.size();i++){
        int count =0;
        for(int j= 0;j<potions.size();j++){
            if(spells[i]*potions[j]>=success){
                cout<<spells[i]<<" "<<potions[j]<<" "<<success<<" "<<count<<endl;
                count++;
            }
            
        }
        ans.push_back(count);
    }
    return ans;
    
}

//aproach 2
vector<int> successfulPairs(vector<int>& spells, vector<int>& potions, long long success) {
    
    sort(potions.begin(),potions.end());
    vector<int> ans;

    for(int i= 0;i<spells.size();i++){
        int count =0;
        for(int j= 0;j<potions.size();j++){
            if((long long)(spells[i]*potions[j])>=success){
                cout<<spells[i]<<" "<<potions[j]<<" "<<success<<" "<<count<<endl;
                count++;
            }
            
        }
        ans.push_back(count);
    }
    return ans;
    
}

//approach
vector<int> successfulPairs(vector<int>& spells, vector<int>& potions, long long success) {
        sort(potions.begin(), potions.end());
        int m = potions.size();
        vector<int> result;

        for (int spell : spells) {
            long long needed = (success + spell - 1) / spell; // ceil(success / spell)
            
            // Find the first potion >= needed
            int index = lower_bound(potions.begin(), potions.end(), needed) - potions.begin();

            // Count of valid potions
            result.push_back(m - index);
        }

        return result;
    }

//approach - main
vector<int> successfulPairs(vector<int>& spells, vector<int>& potions, long long success) {
    vector<int> res;
    int n = potions.size();
    sort(potions.begin(), potions.end());

    for (int spell: spells){
        int s = 0;
        int e = n - 1;
        while (s <= e){
            int mid = s+(e-s)/2;
            long long product = (long long)spell * (long long)potions[mid];
            if (product >= success){
                e = mid -1;
            } else {
                s = mid + 1;
            }
        }
        res.push_back(n-s);
    }
    return res;
}


int main() {
    vector<int> spells = {5,1,3};
    vector<int> potions = {1,2,3,4,5};
    long long success = 7;

    vector<int> ans= successfulPairs(spells,potions,success);

    for( auto i:ans){
        cout<<i<<" ";
    }
    
    return 0;
}
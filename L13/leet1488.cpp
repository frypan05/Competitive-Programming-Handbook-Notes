#include<bits/stdc++.h>
using namespace std;

vector<int> avoidFlood(vector<int>& rains) {
        int n = rains.size();
        vector<int> ans;
        int fullLake=0;
        for(int i =0;i<n;i++){
            if(rains[i]>0){
                ans.push_back(-1);
                fullLake=rains[i];
            }
            if(rains[i]==0){
                ans.push_back(fullLake);
                fullLake--;
            }
        }
    return ans;
}

//approach 2
vector<int> avoidFlood(vector<int>& rains) {
    int n = rains.size();
    vector<int> ans(n);
    // Map to store which lake is full and on which day it became full {lake -> day_index}
    unordered_map<int, int> full_lakes;
    // A sorted set to store the indices of available dry days
    set<int> dry_days;

    for (int i = 0; i < n; ++i) {
        if (rains[i] == 0) {
            // It's a dry day. Add its index to our available dry days.
            dry_days.insert(i);
        } else {
            int lake = rains[i];
            // On a rainy day, we can't dry anything, so ans[i] is -1.
            ans[i] = -1;

            // Check if the lake that is about to get rain is already full.
            if (full_lakes.count(lake)) {
                // This lake is full. We need to find a dry day to empty it.
                // The dry day must have occurred *after* the lake was last filled.
                int last_rain_day = full_lakes[lake];

                // Find the earliest possible dry day *after* the last_rain_day.
                // upper_bound finds the first element strictly greater than last_rain_day.
                auto it = dry_days.upper_bound(last_rain_day);

                // If no such dry day exists in our set, a flood is unavoidable.
                if (it == dry_days.end()) {
                    return {}; // Return an empty vector for the flood case.
                }

                // We found a suitable dry day. Let's use it.
                int dry_day_index = *it;
                ans[dry_day_index] = lake; // On this day, we dry the lake.
                dry_days.erase(it); // This dry day is now used, so remove it.
            }

            // Update the map to show that this lake is now full as of the current day 'i'.
            full_lakes[lake] = i;
        }
    }

    // For any dry days that were not used to prevent a flood,
    // we can just dry any lake (e.g., lake 1).
    for (int day_index : dry_days) {
        ans[day_index] = 1;
    }

    return ans;
}

//approach 3
vector<int> avoidFlood(vector<int>& rains) {
        int n=rains.size();
        unordered_map<int,int>lake;
        for(int i=0;i<n;i++){
            if(rains[i]){
                if(lake.count(rains[i])){
                    int pIdx=lake[rains[i]];
                    int find=0;
                    for(int j=pIdx;j<i;j++){
                        if(!rains[j]){
                            rains[j]=rains[i];
                            find=1;
                            break;
                        }
                    }
                    if(find==0) return {};
                    lake[rains[i]]=i;
                    rains[i]=-1;

                }
                else{
                    lake[rains[i]]=i;
                    rains[i]=-1;
                }

            }

        }
        for(int i=0;i<n;i++){
            if(rains[i]==0) rains[i]=1;
        }
        
        return rains;
        
    }

int main() {

    vector<int> rains = {1,2,3,4};
    vector<int> ans = avoidFlood(rains);
    for(auto i:ans){
        cout<<i<<" ";
    }
    
    return 0;
}
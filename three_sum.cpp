#include <bits/stdc++.h>
using namespace std;

vector<vector<int>> triplet (int n, vector<int> &num){
    //bruteforce.

    set<vector<int>> st;
    for (int i=0; i <n; i++){
        for (int j = i+1; j<n; j++){
            for (int k= j+1; k < n; k++){
                if (num[i] + num[j] + num[k] == 0){

                    //these three lines ensure that all the unique elements are stored.
                    vector<int> temp = {num[i], num[j], num[k]};
                    sort(temp.begin(), temp.end());
                    st.insert(temp);

                }
            }
        }
    }
    vector<vector<int>> ans(st.begin(), st.end());
    return ans;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    if (!(cin >> n)) return 0;
    vector<int> num(n);
    for (int i = 0; i < n; ++i) cin >> num[i];

    auto res = triplet(n, num);
    for (const auto &t : res) {
        for (size_t i = 0; i < t.size(); ++i) {
            if (i) cout << ' ';
            cout << t[i];
        }
        cout << '\n';
    }

    return 0;
}

#include<bits/stdc++.h>
using namespace std;

unordered_map<char, vector<string>> grammar;
unordered_map<char, set<char>> first;        
unordered_map<char, bool> visited; 

void findFirst(char X) {
    // If already computed, skip
    if (visited[X]) return;
    visited[X] = true;

       for (string prod : grammar[X]) {
        for (int i = 0; i < prod.size(); i++) {
            char symbol = prod[i];

            
            if (!isupper(symbol)) { 
                first[X].insert(symbol);
                break;
            }
        
            else {
                findFirst(symbol);
                bool hasEpsilon = false;

                
                for (char c : first[symbol]) {
                    if (c == '#') hasEpsilon = true; // '#' denotes ε
                    else first[X].insert(c);
                }

                
                if (!hasEpsilon) break;

                
                if (hasEpsilon && i == prod.size() - 1)
                    first[X].insert('#');
            }
        }
    }
}


int main() {
    int n;
    cout << "Enter number of productions: ";
    cin >> n;
    cout << "Use # for epsilon (ε)\n";
    cout << "Enter productions (e.g., E->TR | T):\n";


    for (int i = 0; i < n; i++) {
        string rule;
        cin >> rule;
        char lhs = rule[0];
        string rhs = rule.substr(3); 

        
        stringstream ss(rhs);
        string prod;
        while (getline(ss, prod, '|')) {
            grammar[lhs].push_back(prod);
        }
    }

    
    for (auto it : grammar) {
        if (!visited[it.first]) findFirst(it.first);
    }

    
    cout << "\nFIRST sets:\n";
    for (auto it : first) {
        cout << "FIRST(" << it.first << ") = { ";
        for (char c : it.second) cout << c << " ";
        cout << "}\n";
    }

    return 0;
}
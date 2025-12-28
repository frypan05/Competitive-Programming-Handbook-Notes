#include<bits/stdc++.h>
using namespace std;

void eliminateLeftRecursion(string nonTerminal, vector<string> prods) {
    vector<string> alpha; 
    vector<string> beta;   

    for (string prod : prods) {
        if (prod[0] == nonTerminal[0]) {
        
            alpha.push_back(prod.substr(1));  
        } else {
            beta.push_back(prod);
        }
    }

    if (alpha.empty()) {
    
        cout << nonTerminal << " -> ";
        for (int i = 0; i < prods.size(); i++) {
            cout << prods[i];
            if (i != prods.size() - 1)
                cout << " | ";
        }
        cout << endl;
    } else {
        string newNT = nonTerminal + "'";  

        cout << nonTerminal << " -> ";
        for (int i = 0; i < beta.size(); i++) {
            cout << beta[i] << newNT;
            if (i != beta.size() - 1)
                cout << " | ";
        }
        cout << endl;

        cout << newNT << " -> ";
        for (int i = 0; i < alpha.size(); i++) {
            cout << alpha[i] << newNT << " | ";
        }
        cout << "#\n"; 
    }
}


int main() {
    int n;
    cout << "Enter number of prods: ";
    cin >> n;

    map<string, vector<string>> grammar;
    cout << "Enter prods (example: E->E+T|T):\n";

    for (int i = 0; i < n; i++) {
        string s;
        cin >> s;
        string lhs = s.substr(0, s.find("->"));
        string rhs = s.substr(s.find("->") + 2);

        stringstream ss(rhs);
        string prod;
        while (getline(ss, prod, '|')) {
            grammar[lhs].push_back(prod);
        }
    }

    cout << "\nGrammar after eliminating left recursion:\n";
    for (auto &entry : grammar) {
        eliminateLeftRecursion(entry.first, entry.second);
    }

    return 0;
}



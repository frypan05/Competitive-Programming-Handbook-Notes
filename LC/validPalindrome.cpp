#include <bits/stdc++.h>
using namespace std;

/*
 Notes:
 isalnum is a c standard library function (<cctype> and <ctype.h>)
 - here we will use alnum: "alphanumeric characters."
 - we will use two pointers to compare characters from both ends of the string.
 - we will skip the non alphanumeric characters like ^&*()" " space etc.
 - we will convert all characters to lowercase before comparison.
*/

class Solution {
public:
    bool isPalindrome(const string &s) {
        int i = 0, j = (int)s.size() - 1;
        while (i < j) {
            // advance i to next alnum or past j
            while (i < j && !isalnum(static_cast<unsigned char>(s[i]))) ++i;
            // advance j to prev alnum or before i
            while (i < j && !isalnum(static_cast<unsigned char>(s[j]))) --j;

            if (i < j) {
                char a = tolower(static_cast<unsigned char>(s[i]));
                char b = tolower(static_cast<unsigned char>(s[j]));
                if (a != b) return false;
                ++i; --j;
            }
        }
        return true;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string line;
    // Read one full line from stdin
    if (!getline(cin, line)) {
        // No input provided; exit with success
        return 0;
    }

    // On Windows, lines may end with '\r'. Remove it if present.
    if (!line.empty() && line.back() == '\r') line.pop_back();

    Solution sol;
    bool result = sol.isPalindrome(line);
    cout << (result ? "true" : "false") << '\n';

    return 0;
}

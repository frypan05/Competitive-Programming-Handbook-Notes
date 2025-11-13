#include <bits/stdc++.h>
using namespace std;
int main() {
    string day;
    int n;
    cout << "enter the day: " << endl;
    cin >> day;
    cin >> n;
    cout << (n / 7) + (day == "sun");
    return 0;
}

#include <iostream>
#include <vector>

using namespace std;

vector<int> rearrangeAlternate(const vector<int>& arr) {
    vector<int> pos, neg;

    // Separate positives and negatives, preserving order
    for (int num : arr) {
        if (num >= 0)
            pos.push_back(num);
        else
            neg.push_back(num);
    }
    
    vector<int> result;
    int i = 0, j = 0;

    // Alternate positive and negative
    while (i < pos.size() && j < neg.size()) {
        result.push_back(pos[i++]);
        result.push_back(neg[j++]);
    }

    // Append remaining positives
    while (i < pos.size())
        result.push_back(pos[i++]);

    // Append remaining negatives
    while (j < neg.size())
        result.push_back(neg[j++]);

    return result;
}

int main() {
    vector<int> arr = {5, 3, -1, -2, 8, 9, -7};

    vector<int> result = rearrangeAlternate(arr);

    cout << "Rearranged array: ";
    for (int num : result) {
        cout << num << " ";
    }

    cout << endl;
    return 0;
}

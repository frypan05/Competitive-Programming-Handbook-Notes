#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int getMaxProfit(const vector<int>& prices) {
    if (prices.size() < 2) return 0;

    int minPrice = prices[0];
    int maxProfit = 0;

    for (int i = 1; i < prices.size(); i++) {
        int profit = prices[i] - minPrice;
        maxProfit = max(maxProfit, profit);
        minPrice = min(minPrice, prices[i]);
    }

    return maxProfit;
}

int main() {
    vector<int> prices = {6, 1, 8, 3, 9, 7, 2};
    int profit = getMaxProfit(prices);
    cout << "Maximum Profit: " << profit << endl;
    return 0;
}

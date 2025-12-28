#include <bits/stdc++.h>
using namespace std;


class Solution {
    public:
        int maxArea(vector <int> &height){
            int l = 0;
            int r = (int)height.size()-1;
            int best = 0;

            while (l < r){
                long long h = min(height[l],height[r]);
                long long width = r-l;
                long long area = h* width;

                if (area > best) best = area;

                if (height[l] < height [r]){
                    ++l;
                }
                else{
                    --r;
                }
            }
            return best;
        }
}

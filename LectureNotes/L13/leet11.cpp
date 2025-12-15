#include<bits/stdc++.h>
using namespace std;


int maxArea(vector<int>& height) {
    int left = 0;
    int right  = height.size()-1;
    int max_area = 0;
    
    while (left < right){
        int area =0;
        if(height[left]<height[right]){
            area  = height[left]*(right-left);
            left ++;
        }else{
            area  = height[right]*(right-left);
            right --;
        }
        if(max_area<area){
            max_area = area;
        }
    }
    return max_area;
}    


int main() {
    
    return 0;
}
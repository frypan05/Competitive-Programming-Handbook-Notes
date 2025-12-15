#include<bits/stdc++.h>
using namespace std;

void merge(int* nums1 , int m ,int* nums2,  int n ){

    

    vector<int> ans;
    
    if(m!=0){
      for(int i =0;i<m;i++){
        ans.push_back(nums1[i]);
        }
    }
    
    if(n!=0){
        for(int i =0;i<n;i++){
            ans.push_back(nums2[i]);
        }    
    }
    


    sort(ans.begin(),ans.end());

    for(int i=0;i<m+n;i++){
        nums1[i]=ans[i];
    }


}


int main() {

    int nums1[6] = {1,2,3,0,0,0};
    int m = 3;
    int nums2[3] = {2,5,6};
    int n = 3;

    merge(nums1,m,nums2,n);

    for(int i = 0 ;i<m+n;i++){
        cout<<nums1[i]<<" ";
    }

    
    return 0;
}
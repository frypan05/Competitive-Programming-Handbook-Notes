#include<bits/stdc++.h>
using namespace std;

vector<int> findArrSum(vector<int>&a,int n,vector<int>&b,int m){
    int i=n-1;
    int j=m-1;

    vector<int> ans;
    int carry=0;

    while(i>=0&&j>=0){
        int val1=a[i];
        int val2=b[j];

        int sum = val1+val2+carry;
        carry=sum/10;
        sum = sum%10;

        ans.push_back(sum);
        i--;
        j--;
    }

    //first case

    while(i>=0){
        int sum =  a[i]+carry;
        carry=sum/10;
        sum = sum%10;

        ans.push_back(sum);
        i--;
    }

    //second case

    while(j>=0){
        int sum =  a[j]+carry;
        carry=sum/10;
        sum = sum%10;

        ans.push_back(sum);
        j--;
    }

    //third case

    while(carry!=0){
        int sum =  carry;
        carry=sum/10;
        sum = sum%10;

        ans.push_back(sum);

    }

    int s=0;
    int e=ans.size()-1;
    while(s<e){
        swap(ans[s++],ans[e--]);
    }

    return ans;
}

void printarr(vector<int> & nums){
        for(int i=0;i<nums.size();i++){
        cout<<nums[i]<< " ";
    }
}

int main() {

    vector<int> a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    a.push_back(4);
    a.push_back(5);
    a.push_back(6);
    a.push_back(7);
    a.push_back(8);


    vector<int> b;
    b.push_back(1);
    b.push_back(2);
    b.push_back(3);
    b.push_back(4);
    b.push_back(5);
    b.push_back(6);
    b.push_back(7);
    b.push_back(8);

    vector<int> ans=  findArrSum(a,a.size(),b,b.size());

    printarr(ans);
    
    return 0;
}



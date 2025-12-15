//Problem statement
//You are given two arrays 'A' and 'B' of size 'N' and 'M' respectively. Both these arrays are sorted in non-decreasing order. You have to find the intersection of these two arrays.
//
//Intersection of two arrays is an array that consists of all the common elements occurring in both arrays.
//
//Note :
//1. The length of each array is greater than zero.
//2. Both the arrays are sorted in non-decreasing order.
//3. The output should be in the order of elements that occur in the original arrays.
//4. If there is no intersection present then return an empty array.


#include<bits/stdc++.h>
using namespace std;

//here the resultant vector would have duplicates
//approach 1
vector<int> arrayIntersection(vector<int> arr1,vector<int> arr2,int n,int m){

    vector<int> ans;

    for(int i=0;i<n;i++){
        int element = arr1[i];
        for(int j=0;j<m;j++){
            if(element<arr2[j]){
                break;
            }
            if(element == arr2[j]){
                ans.push_back(element);
                arr2[j]= INT_MIN;
                break;  
            }      
        }    
    }
    return ans;
}
// approach 2 (two pointer approach)
vector<int> array_Intersection(vector<int> arr1,vector<int> arr2,int n,int m){
    
    int i=0,j=0;
    vector<int> ans;
    while(i<n&&j<m){
        if(arr1[i]==arr2[j]){
            ans.push_back(arr1[i]);
            i++;
            j++;
        }
        else if(arr1[i]<arr2[j]){
            i++;
        }
        else{
            j++;
        }
    }
     //rremoves dupes from ans vector
    //ans.erase(unique(ans.begin(), ans.end()), ans.end());

    return ans;
}



int main() {
    vector<int> a={2,3,3,4};
    int n=a.size();
    vector<int> b={1,3,3,4,5};
    int m =b.size();

    array_Intersection(a,b,n,m);

    for(int i=0;i<array_Intersection(a,b,n,m).size();i++){
        cout<<array_Intersection(a,b,n,m)[i];
    }
    return 0;
}
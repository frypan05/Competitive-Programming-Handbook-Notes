#include<bits/stdc++.h>
using namespace std;


int largest_row_sum(int arr[][4],int i,int j){
    int maxi = INT_MIN;
    int rowindex=-1;

    for(int i=0;i<3;i++){
        int sum=0;
        for(int j=0;j<4;j++){
            sum+=arr[i][j];
        }
        if(sum>maxi){
        maxi=sum;
        rowindex=i;
        }
    }
    cout<<" max sum is "<<maxi;
    return rowindex;
}

void row_sum(int arr[][4],int i,int j){

    for(int i=0;i<3;i++){
        int sum=0;
        for(int j=0;j<4;j++){
            sum+=arr[i][j];
        }
        cout<<sum<<endl;
    }
}

void col_sum(int arr[][4],int i,int j){

    for(int j=0;j<4;j++){
        int sum=0;
        for(int i=0;i<3;i++){
            sum+=arr[i][j];
        }
        cout<<sum<<endl;
    }
}

int main() {

    int arr[3][4]={1,2,3,4,5,6,7,8,9,10,11,12};

    cout<<" max row is at index "<< largest_row_sum(arr,3,4)<<endl;
    
    return 0;
} 
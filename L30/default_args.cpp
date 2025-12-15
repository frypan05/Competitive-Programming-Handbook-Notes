#include<bits/stdc++.h>
using namespace std;

void print(int arr[], int n , int start = 0){
    for (int i=start;i<n;i++){
        cout<<arr[i]<<endl;
    }
}

int add(int x , int y, int z=0,int w=0){
    return x+y+z+w;
}

int main() {

    int arr[5]={1,4,7,8,9};
    int size = 5;
    print(arr,size);
    cout<<endl;
    print(arr,size,2);



    cout<<add(10,20)<<endl;
    cout<<add(10,20,30)<<endl;
    cout<<add(10,20,30,40)<<endl;
    
    return 0;
}


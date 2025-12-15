#include<bits/stdc++.h>
using namespace std;

int getsum(int *arr,int n){
    int sum=0;
    for(int i=0;i<n;i++){
        sum+=arr[i];
    }
    return sum;
}

int main() {

    //char ch = 'q';
    //cout<<sizeof(ch)<<endl;
    //char *c = &ch;
    //cout<<sizeof(c)<<endl;

    int n;
    cin>>n;

    //variable size array
    int*arr = new int[n];
    //n*4 memory is used from the heap

    //taking inputs
    for(int i = 0;i<n;i++){
        cin>>arr[i];
    }

    int ans = getsum(arr,n);
    cout<<"answer is "<<ans <<endl;


    //case 1
    while (true){
        int i= 5;
    }

    //case2
    while (true){
        int *ptr = new int;
    }
    

    return 0;
}
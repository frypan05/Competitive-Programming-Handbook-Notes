#include<bits/stdc++.h>
using namespace std;

int main() {

    //for n*n array
    // int n;
    // cin>>n;

    // int ** arr= new int*[n];

    // for(int i = 0;i<n;i++){
    //     arr[i]= new int[n];
    // }
    // //creation done


    //for m*n array
    int row;
    cin>>row;

    int col;
    cin>>col;

    int ** arr= new int*[row];

    for(int i = 0;i<row;i++){
        arr[i]= new int[col];
    }
    //creation done


    //input
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            cin>>arr[i][j];
        }
    }

    //output
    for(int i=0;i<row;i++){
        for(int j=0;j<col;j++){
            cout<<arr[i][j]<<" ";
        }cout<<endl;
    }

    //releasing

    for(int i = 0;i<row;i++){//arrays that were being pointed by the array of pointers
        delete [] arr[i];
    }

    delete []arr; // array of pointers
    

    return 0;
}
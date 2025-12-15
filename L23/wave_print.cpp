//https://www.youtube.com/redirect?event=video_description&redir_token=QUFFLUhqbm9CU2ZUNTgzVmJaSGd5bTNFd1JKTjB0NFhqd3xBQ3Jtc0tuMldRUGVRcGs4Y042d01MWDVHb0FwM0xIMjZQc2oyY1hlZG5zdE1VMEhqaW1fREl1TnVsMUE3SF9zc1R6QTZzNXJBZUhpekpqc0pMbll1Y0gtZGVnN1JOVEFkcFhGV0xIOHRJaUNCeVh1c1hWaWVYNA&q=https%3A%2F%2Fbit.ly%2F329Le3K&v=1CdolnvxLs0

#include<bits/stdc++.h>
using namespace std;


// void wave_print(int arr[][3]){
//     int num=0;
//     for( int col = 0 ;col<3;col++){
        
//         if(num%2==0){
//             for(int row=0;row<3;row++){
//             cout<<arr[row][col]<<" ";
//             }
//             num++;
//         }
//         else{
//             for(int row=2;row>=0;row--){
//             cout<<arr[row][col]<<" ";
//             }
//             num++;
//         }     
// }
// }


void wave(int arr[][3],int nrows,int mcols){//O(n*m)
    for( int col = 0 ;col<mcols;col++){
        if(col&1){
            //odd index -> bottom to top  
            for(int row=nrows-1;row>=0;row--){
                cout<<arr[row][col]<<" ";
            }    
        }else{//0 or even index
            for(int row=0;row<nrows;row++){
                cout<<arr[row][col]<<" ";
            }
        }      
    }
}

void wave_print(int arr[][3]){
    
    for( int col = 0 ;col<3;col++){
        
        if(col%2==0){
            for(int row=0;row<3;row++){
            cout<<arr[row][col]<<" ";
            }   
        }
        else{
            for(int row=2;row>=0;row--){
            cout<<arr[row][col]<<" ";
            }
        }      
    }
}




int main() {

    int arr[3][3]={{3,4,11},{2,12,1},{7,8,7}};

    //wave_print(arr);
    wave(arr,3,3);
    
    return 0;
}





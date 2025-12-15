#include<bits/stdc++.h>
using namespace std;


void print(int *p){
    cout<<p<<endl;

}

void val_update(int *p){
 
    *p =*p+1;
}

void add_update(int *p){
    p = p+1;
    cout<<" inside "<<p <<endl;

 
}

int getSum( int *arr/*int arr[]*/ , int n){

    cout<<"SIZE : "<< sizeof(arr)<<endl;//will be equal 4 as we pass arr as a pointer in the function

    int sum = 0;
    for(int i=0; i<n;i++){
        sum+= i[arr];
    }
    return sum;


}


int main() {

    int value =5;
    int *p = &value;

    //print(p);

    cout<<" before "<<*p<<endl;
    val_update(p);
    cout<<" after "<<*p <<endl;
    
    cout<<" before "<<p<<endl;
    add_update(p);
    cout<<" after "<<p <<endl;


    int  arr[6]={1,2,3,4,5,8};

    cout<<"sum is "<<getSum(arr,5)<<endl;
    //we not passing array in a function instead we are passing a pointer
    //that points to the first element of the array

    cout<<"sum is "<<getSum(arr+3,3)<<endl;
    
    return 0;
} 
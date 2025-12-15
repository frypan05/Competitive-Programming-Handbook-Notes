#include<iostream>
using namespace std;

int main(){
    
    int n;
    cin>>n;
    cout<<n<<endl;
    char ch[n];
    cin>>ch;
    cout<<ch<<endl;
    
    int count=0;
    //for(int i=0;i<sizeof(ch)/sizeof(ch[0]);i++){
    //    cout<<ch[i]<<endl;
    //}
    if(count>n)
    //for(int i=0;i<sizeof(ch)/sizeof(ch[0]);i++){
    //    cout<<ch[i]<<endl;
    //}
    cout<<ch[0]<<count<<ch[n-1];
    //for(int i=0;i<ch[n];i++){
    //    cout<<ch[n]<<count++<<endl;
    //    
    //}
    

    return 0;
}
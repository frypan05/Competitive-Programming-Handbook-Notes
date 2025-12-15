#include<iostream>
#include<climits> //INT_MAX/MIN is defined in the <climits> header in C++

using namespace std;

//leetcode 7 - reverse an int

int main(){
    int n=3546;
    int ans=0;
    int digit;

    while(n!=0){
        digit= n%10;

        if(ans>(INT_MAX/10) || ans<(INT_MIN/10)){       //case when int range is exceeded
            return 0;
        }
        
        ans=ans*10+digit;
        n=n/10;
        
    }

    cout<<ans;
    
    return 0;
}


//bool isPalindrome(int x) {    
//        int n =x;
//        long long ans=0;
//        int digit;
//        if(n<0){
//            return false;}
//        while(n!=0){
//            ans=ans*10+n%10;
//            n/=10;}
//        return ans==x;     
//    }          
//};

#include<bits/stdc++.h>
using namespace std;

//TLE
//The loop will run until the time limit is reached, 
//or until numBottles overflows the integer limit and wraps around, which is why you get a TLE.
// int maxBottlesDrunk(int numBottles, int numExchange) {
//         int bottlesDrank = numBottles;
//         cout<<"exchange value: "<<numExchange<<endl;
//         cout<<"Bottles Drunk: "<<bottlesDrank<<endl<<endl;
        
//         cout<<"Bottles Drunk: "<<bottlesDrank<<endl;
//         bottlesDrank+=numBottles/numExchange ;
//         cout<<"Bottles Drunk: "<<bottlesDrank<<endl;
//         while(numBottles/numExchange>=1){
//             numBottles += numBottles/numExchange;
//             numExchange++;
//         }
//         numBottles += numBottles%numExchange;
            
//         return bottlesDrank;
// }

//https://leetcode.com/problems/water-bottles-ii/solutions/7242280/the-math-solution-finally-explained-deri-jalx/
//https://leetcode.com/problems/water-bottles-ii/solutions/7243357/2-approach-video-explanation-optimal-approach-with-proper-mathematical-proof/
int maxBottlesDrunk(int numBottles, int numExchange) {
    int b = 2*numExchange-3;
    int c = 2*(numBottles-1);
    double n = (-b + sqrt(b*b+4*c))/2; //only taking positive roots

    return numBottles + floor(n);
}



int main() {
    cout<<maxBottlesDrunk(13,6);
    
    return 0;
}
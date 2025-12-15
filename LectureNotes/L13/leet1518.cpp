#include<bits/stdc++.h>
using namespace std;

int numWaterBottles(int numBottles, int numExchange) {
        int bottlesDrank = numBottles;
        while(numBottles/numExchange >=1){
            cout<<bottlesDrank<<endl;
            bottlesDrank+=numBottles/numExchange ;
            cout<<bottlesDrank<<endl;
            numBottles = numBottles/numExchange + numBottles%numExchange;
            cout<<numBottles<<endl;
        }
        return bottlesDrank;
    }

int main() {
    cout<<numWaterBottles(15,4);
    
    return 0;
}
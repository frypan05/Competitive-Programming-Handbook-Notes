//A map is a data structure that stores key-value pairs, allowing for 
//efficient retrieval of values based on their associated keys. 
//Each key in a map is unique, and the values can be accessed, inserted, 
//or modified using these keys.

//maps are sorted
//normal maps are implemented with balanced trees (complexity O(logn))
//unordered maps are implemented with hash tables (search complexity O(1))

#include<iostream>
//#include<unordered_map>
#include<map>
using namespace std;

int main() {

    map<int,string> m;

    m[1]="babbar";
    m[13]="kumar";
    m[2]="love";

    m.insert({5,"bheem"});

    cout<<"before erase "<<endl;
    for(auto i:m){
        cout<<i.first<<" "<<i.second<<endl;
    }

    cout<<"finding 13 :"<<m.count(13)<<endl;
    cout<<"finding -13 :"<<m.count(-13)<<endl;

    //m.erase(13);
    cout<<"after erase "<<endl;
        for(auto i:m){
        cout<<i.first<<" "<<i.second<<endl;
    }


    auto it = m.find(5);
    for(auto i=it;i!=m.end();i++){
        cout<<(*i).first<<endl;
    }







    
    return 0;
}
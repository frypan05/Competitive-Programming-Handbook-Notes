#include<iostream>
#include<map>
#include<unordered_map>
using namespace std;

//ordered map -> maintains order of insertion
//unordered map -> doesnt maintain order of insertion

int main(){

    //creation
    map<string,int> m;

    //insertion
    //1
    pair<string,int> p = make_pair("idk",3);
    m.insert(p);

    //2
    pair<string,int> q("something",2);
    m.insert(q);

    //3
    m["mera"] = 1;
    m["mera"] =1;
    m["dsa"] =4;

    //searching - will return value of that key
    cout<<m["mera"]<<endl; //print value of key provided
    cout<<m.at("something")<<endl;

    //cout<<m.at("hehe")<<endl; //searching undefined key //out of range error
    cout<<m["hehe"]<<endl; // but this will return 0 //as this method created an entry for the unknown key with value 0
    cout<<m.at("hehe")<<endl; //but now this gives 0 aswell 

    //size
    cout<<m.size()<<endl; 

    //to check presence of a key
    cout<<m.count("bro")<<endl; //Returns Number of elements with specified key. // in case of map this will return 0 or 1
    cout<<m.count("mera")<<endl;

    //erase
    m.erase("mera"); //to remove a <k,V> entry
    cout<<m.size()<<endl;

    //map traverse
    for(auto i:m){
        cout<<i.first<<" "<<i.second<<endl;
    }
    cout<<endl;

    //iterator
    // cout<<"for unordered Map"<<endl;
    // unordered_map<string,int>::iterator it = m.begin();
    // while(it != m.end()){
    //     cout<<it->first<<" "<<it->second<<endl;
    //     it++;
    // }

    cout<<"for ordered Map"<<endl;
    map<string,int>::iterator it = m.begin(); 
    while(it != m.end()){
        cout<<it->first<<" "<<it->second<<endl;
        it++;
    }
     
    return 0;
}
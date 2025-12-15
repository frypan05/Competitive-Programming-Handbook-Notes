//set - only unique elements are stored
//implemented using bst
//modification is not allowed in set
//elements are returned in sorted order

#include<iostream>
//#include<unordered_set>
#include<set>
using namespace std;

int main() {

    set<int> s;

    s.insert(5); //insert operation has timecomplexity O(logn )
    s.insert(5);
    s.insert(5);
    s.insert(1);
    s.insert(6);
    s.insert(6);
    s.insert(0);
    s.insert(0);
    s.insert(0);

    for(auto i:s){
        cout<<i<<endl;
    }cout<<endl; 

    set<int> :: iterator it = s.begin();
    it++;

    s.erase(it); //O(logn)
    for(auto i:s){
        cout<<i<<endl;
    }cout<<endl;

    cout<<"5 is present or not "<<s.count(5)<<endl; //count() to check if element is present
    cout<<"-5 is present or not "<<s.count(-5)<<endl;
    

    set<int>::iterator itr = s.find(5); //find method gives us reference to that value//O(logn)
    cout<<"value present at itr "<<*it<<endl;

    for(auto it=itr;it!=s.end();it++){
        cout<<*it<<" ";
    }cout<<endl;  


    
    return 0;
}
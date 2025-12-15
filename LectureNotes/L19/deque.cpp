//Deque - double-ended queue
//It is a linear data structure that allows insertion
//and deletion of elements from both the front and back.
//It can operate both as a stack (LIFO) and a queue (FIFO).
//it is dynamic

#include<iostream>
#include<deque>

using namespace std;

int main() {

    deque<int> d;

    d.push_back(1);
    d.push_front(2);  

    for(int i:d){
        cout<<i<<" ";
    }  

    //d.pop_back();
    //d.pop_front();
    cout<<endl;
    //for(int i:d){
    //    cout<<i<<" ";
    //}  

    cout<<"print first index element "<<d.at(1)<<endl;

    cout<<"front "<<d.front()<<endl;
    cout<<"back "<<d.back()<<endl;

    cout<<"empty or not "<<d.empty()<<endl;

    for(int i:d){
        cout<<i<<" ";
    }cout<<endl;
    cout<<distance(d.begin(),d.end())<<endl; 
    //normal d.begin-d.end() is not applicable here

    cout<<"before erase "<<d.size()<<endl;
    d.erase(d.begin(),d.begin()+1);//range of elements to be erased//parameter are needed for deque 
    cout<<"after erase "<<d.size()<<endl;
    for(int i:d){
        cout<<i<<" ";
    }cout<<endl;





    return 0;
}  

#include <bits/stdc++.h>
using namespace std;

class LFUCache {
private:
    unordered_map<int,pair<int,int>>keyVals; // key->{value,cnt}
    unordered_map<int,list<int>::iterator> keyIters; // key->iterator
    unordered_map<int,list<int>>keyCnts; // cnt->list
    int m_capacity;
    int mincnt;
    void touch(int key){
        int cnt=keyVals[key].second;
        keyVals[key].second++;
        keyCnts[cnt].erase(keyIters[key]);
        keyCnts[cnt+1].push_back(key);
        keyIters[key]=--keyCnts[cnt+1].end();

        while(keyCnts[mincnt].empty()){
            mincnt++;
        }
    }
public:
    LFUCache(int capacity):m_capacity(capacity),mincnt(0) {}
    
    int get(int key) {
        if(!keyVals.contains(key)) return -1;
        touch(key);
        return keyVals[key].first;
    }
    
    void put(int key, int value) {
        if(m_capacity<=0) return;

        if(keyVals.contains(key)){
            keyVals[key].first=value;
            touch(key);
        }
        else{
            if(m_capacity==keyVals.size()){
               auto lastkey= keyCnts[mincnt].front();
               keyVals.erase(lastkey);
               keyIters.erase(lastkey);
               keyCnts[mincnt].pop_front();
            }
            keyVals[key]={value,1};
            keyCnts[1].push_back(key);
            keyIters[key]=--keyCnts[1].end();
            mincnt=1;
        }
    }
};

int main() {
    string op;
    vector<int> params;
    LFUCache* cache = nullptr;
    
    while(cin >> op) {
        if(op == "LFUCache") {
            int capacity;
            cin >> capacity;
            cache = new LFUCache(capacity);
            cout << "null" << endl;
        }
        else if(op == "put") {
            int key, value;
            cin >> key >> value;
            cache->put(key, value);
            cout << "null" << endl;
        }
        else if(op == "get") {
            int key;
            cin >> key;
            cout << cache->get(key) << endl;
        }
    }
    
    delete cache;
    return 0;
}


// LFUCache 2
// put 1 1
// put 2 2
// get 1
// put 3 3
// get 2
// get 3
// put 4 4
// get 1
// get 3
// get 4
#include <bits/stdc++.h>

using namespace std;
class LRUKCache {
private:
    int m_capacity;
    int m_k;
    int m_timestamp;
    
    // 存储缓存的key-value对
    list<pair<int,int>> cache;  // {key->value}
    unordered_map<int,list<pair<int,int>>::iterator> cache_map; // {key->iterator}
    
    // 存储历史访问记录
    unordered_map<int, queue<int>> history;  // {key->[timestamps]}
    list<int> history_list;  // 存储历史记录中的key，按最近访问时间排序
    unordered_map<int, list<int>::iterator> history_map;  // {key->iterator}

    void updateHistory(int key) {
        m_timestamp++;
        
        // 更新访问历史
        if (!history.count(key)) {
            history[key] = queue<int>();
            history_list.push_front(key);
            history_map[key] = history_list.begin();
        } else {
            history_list.erase(history_map[key]);
            history_list.push_front(key);
            history_map[key] = history_list.begin();
        }
        
        auto& q = history[key];
        q.push(m_timestamp);
        if (q.size() > m_k) {
            q.pop();
        }
        
        // 如果历史记录超过容量，删除最久未访问的记录
        while (history.size() > m_capacity) {
            int remove_key = history_list.back();
            history_list.pop_back();
            history.erase(remove_key);
            history_map.erase(remove_key);
        }
    }

    bool isKAccessed(int key) {
        return history[key].size() >= m_k;
    }

public:
    LRUKCache(int capacity, int k) : m_capacity(capacity), m_k(k), m_timestamp(0) {}
    
    int get(int key) {
        updateHistory(key);
        
        if (!cache_map.count(key)) return -1;
        
        // 如果在缓存中，更新位置
        cache.splice(cache.begin(), cache, cache_map[key]);
        return cache_map[key]->second;
    }
    
    void put(int key, int value) {
        updateHistory(key);
        
        // 如果key已经在缓存中
        if (cache_map.count(key)) {
            cache_map[key]->second = value;
            cache.splice(cache.begin(), cache, cache_map[key]);
            return;
        }
        
        // 只有当访问次数达到K次时才放入缓存
        if (!isKAccessed(key)) {
            return;
        }
        
        // 如果缓存满了，删除最久未使用的元素
        if (cache.size() == m_capacity) {
            int remove_key = cache.back().first;
            cache_map.erase(remove_key);
            cache.pop_back();
        }
        
        // 放入缓存
        cache.push_front({key, value});
        cache_map[key] = cache.begin();
    }
};
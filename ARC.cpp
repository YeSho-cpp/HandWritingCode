#include <iostream>
#include <list>
#include <unordered_map>
#include <algorithm>

template <typename Key, typename Value>
class ARCache {
private:
    // 缓存容量
    size_t capacity;
    
    // 适应性参数，用于调整 T1 和 T2 的大小
    size_t p = 0;
    
    // 四个列表
    std::list<Key> t1;  // 最近使用一次的页面
    std::list<Key> t2;  // 最近频繁使用的页面
    std::list<Key> b1;  // 从 T1 淘汰的页面历史
    std::list<Key> b2;  // 从 T2 淘汰的页面历史
    
    // 哈希表用于快速查找和访问
    std::unordered_map<Key, std::pair<Value, typename std::list<Key>::iterator>> cache;
    std::unordered_map<Key, typename std::list<Key>::iterator> b1_map;
    std::unordered_map<Key, typename std::list<Key>::iterator> b2_map;
    
    // 从列表中删除指定的键
    void removeFromList(const Key& key, std::list<Key>& lst, 
                        std::unordered_map<Key, typename std::list<Key>::iterator>& map) {
        auto it = map.find(key);
        if (it != map.end()) {
            lst.erase(it->second);
            map.erase(it);
        }
    }
    
    // 在列表头部添加键
    void addToFront(const Key& key, std::list<Key>& lst, 
                    std::unordered_map<Key, typename std::list<Key>::iterator>& map) {
        lst.push_front(key);
        map[key] = lst.begin();
    }
    
    // 替换策略
    void replace(const Key& key, int case_num) {
        // 适应性调整参数 p
        if (case_num == 3 && b1.size() >= b2.size()) {
            p = std::min(p + 1, capacity);
        } else if (case_num == 4 && b2.size() >= b1.size()) {
            p = p > 0 ? p - 1 : 0;
        }
        
        if (t1.size() >= 1 && ((t1.size() > p && (case_num == 3 || case_num == 4)) || 
                               (t1.size() == p && case_num == 2))) {
            // 从 T1 中淘汰
            Key lru_key = t1.back();
            t1.pop_back();
            
            // 将淘汰的页面添加到 B1
            b1.push_front(lru_key);
            b1_map[lru_key] = b1.begin();
            
            // 从缓存中移除
            cache.erase(lru_key);
        } else {
            // 从 T2 中淘汰
            Key lru_key = t2.back();
            t2.pop_back();
            
            // 将淘汰的页面添加到 B2
            b2.push_front(lru_key);
            b2_map[lru_key] = b2.begin();
            
            // 从缓存中移除
            cache.erase(lru_key);
        }
    }
    
public:
    ARCache(size_t size) : capacity(size) {}
    
    // 获取缓存值，如果不存在返回默认构造的值
    Value get(const Key& key) {
        auto it = cache.find(key);
        if (it == cache.end()) {
            return Value{};
        }
        
        // 从当前位置移除
        if (b1_map.find(key) != b1_map.end()) {
            // Case 3: 在 B1 中 - 提升到 T2
            removeFromList(key, b1, b1_map);
            t2.push_front(key);
            it->second.second = t2.begin();
        } else if (b2_map.find(key) != b2_map.end()) {
            // Case 4: 在 B2 中 - 提升到 T2
            removeFromList(key, b2, b2_map);
            t2.push_front(key);
            it->second.second = t2.begin();
        } else if (std::find(t1.begin(), t1.end(), key) != t1.end()) {
            // 从 T1 移动到 T2
            t1.erase(it->second.second);
            t2.push_front(key);
            it->second.second = t2.begin();
        } else {
            // 已经在 T2 中，只需移到前面
            t2.erase(it->second.second);
            t2.push_front(key);
            it->second.second = t2.begin();
        }
        
        return it->second.first;
    }
    
    // 设置或更新缓存值
    void put(const Key& key, const Value& value) {
        // 检查是否已经在缓存中
        if (cache.find(key) != cache.end()) {
            // 更新值并移动
            cache[key].first = value;
            get(key);  // 使用 get 来更新位置
            return;
        }
        
        // 检查缓存是否已满
        if (t1.size() + t2.size() >= capacity) {
            // 需要替换
            int case_num = 1;
            
            // 确定情况
            if (b1_map.find(key) != b1_map.end()) {
                case_num = 3;
            } else if (b2_map.find(key) != b2_map.end()) {
                case_num = 4;
            }
            
            replace(key, case_num);
        }
        
        // 确定在哪个列表中放置新元素
        if (b1_map.find(key) != b1_map.end()) {
            // Case 3: 在 B1 中
            p = std::min(p + 1, capacity);
            removeFromList(key, b1, b1_map);
            t2.push_front(key);
            cache[key] = {value, t2.begin()};
        } else if (b2_map.find(key) != b2_map.end()) {
            // Case 4: 在 B2 中
            p = p > 0 ? p - 1 : 0;
            removeFromList(key, b2, b2_map);
            t2.push_front(key);
            cache[key] = {value, t2.begin()};
        } else {
            // Case 1: 新元素
            if (b1.size() + t1.size() >= capacity) {
                // B1 已满，清除最旧的条目
                if (b1.size() > 0) {
                    Key lru = b1.back();
                    b1.pop_back();
                    b1_map.erase(lru);
                }
            }
            
            // 添加到 T1
            t1.push_front(key);
            cache[key] = {value, t1.begin()};
        }
        
        // 确保 B1 + B2 不超过缓存容量
        while (b1.size() + b2.size() > capacity) {
            if (b1.size() > b2.size()) {
                Key lru = b1.back();
                b1.pop_back();
                b1_map.erase(lru);
            } else {
                Key lru = b2.back();
                b2.pop_back();
                b2_map.erase(lru);
            }
        }
    }
    
    // 检查键是否存在于缓存中
    bool contains(const Key& key) {
        return cache.find(key) != cache.end();
    }
    
    // 返回缓存当前大小
    size_t size() {
        return t1.size() + t2.size();
    }
    
    // 显示当前状态信息
    void printStatus() {
        std::cout << "ARC Status:" << std::endl;
        std::cout << "p = " << p << std::endl;
        std::cout << "T1 size: " << t1.size() << ", T2 size: " << t2.size() << std::endl;
        std::cout << "B1 size: " << b1.size() << ", B2 size: " << b2.size() << std::endl;
    }
};

// 使用示例
int main() {
    ARCache<int, std::string> cache(4);
    
    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    cache.put(4, "four");
    
    cache.printStatus();
    
    std::cout << "Get 1: " << cache.get(1) << std::endl;
    std::cout << "Get 2: " << cache.get(2) << std::endl;
    
    cache.put(5, "five");  // 会触发替换
    
    cache.printStatus();
    
    std::cout << "Contains 3: " << (cache.contains(3) ? "Yes" : "No") << std::endl;
    std::cout << "Contains 5: " << (cache.contains(5) ? "Yes" : "No") << std::endl;
    
    return 0;
}
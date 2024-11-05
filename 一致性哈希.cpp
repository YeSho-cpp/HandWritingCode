#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdlib>
#include <sstream>

class ConsistentHash {
private:
    // 虚拟节点倍数
    int virtualNodes;
    // 哈希环
    std::map<size_t, std::string> ring;
    // 物理节点到虚拟节点的映射
    std::unordered_map<std::string, std::vector<size_t>> nodeToVirtual;
    
    // 哈希函数
    size_t hash(const std::string& key) {
        return std::hash<std::string>{}(key);
    }
    
    // 生成虚拟节点的key
    std::string getVirtualKey(const std::string& node, int index) {
        std::stringstream ss;
        ss << node << "#" << index;
        return ss.str();
    }

public:
    ConsistentHash(int virtualNodeCount = 3) : virtualNodes(virtualNodeCount) {}
    
    // 添加节点
    void addNode(const std::string& node) {
        // 为物理节点创建虚拟节点
        for (int i = 0; i < virtualNodes; ++i) {
            std::string virtualKey = getVirtualKey(node, i);
            size_t hashValue = hash(virtualKey);
            ring[hashValue] = node;
            nodeToVirtual[node].push_back(hashValue);
        }
    }
    
    // 删除节点
    void removeNode(const std::string& node) {
        // 删除该节点的所有虚拟节点
        auto it = nodeToVirtual.find(node);
        if (it != nodeToVirtual.end()) {
            for (size_t hashValue : it->second) {
                ring.erase(hashValue);
            }
            nodeToVirtual.erase(it);
        }
    }
    
    // 获取负责处理key的节点
    std::string getNode(const std::string& key) {
        if (ring.empty()) {
            throw std::runtime_error("Hash ring is empty");
        }
        
        size_t hashValue = hash(key);
        
        // 找到第一个大于等于hashValue的节点
        auto it = ring.lower_bound(hashValue);
        
        // 如果找不到，则返回第一个节点
        if (it == ring.end()) {
            return ring.begin()->second;
        }
        
        return it->second;
    }
    
    // 打印当前环的状态
    void printRing() {
        std::cout << "Current hash ring state:" << std::endl;
        for (const auto& pair : ring) {
            std::cout << "Hash: " << pair.first << " -> Node: " << pair.second << std::endl;
        }
        std::cout << std::endl;
    }
};

// 测试代码
void testConsistentHash() {
    ConsistentHash ch(3);  // 每个物理节点创建3个虚拟节点
    
    // 添加节点
    std::vector<std::string> nodes = {"node1", "node2", "node3", "node4"};
    for (const auto& node : nodes) {
        ch.addNode(node);
    }
    
    // 测试key的分配
    std::vector<std::string> keys = {"key1", "key2", "key3", "key4", "key5"};
    
    std::cout << "Initial distribution:" << std::endl;
    for (const auto& key : keys) {
        std::cout << "Key: " << key << " -> Node: " << ch.getNode(key) << std::endl;
    }
    std::cout << std::endl;
    
    // 删除一个节点
    std::cout << "After removing node2:" << std::endl;
    ch.removeNode("node2");
    for (const auto& key : keys) {
        std::cout << "Key: " << key << " -> Node: " << ch.getNode(key) << std::endl;
    }
    std::cout << std::endl;
    
    // 添加新节点
    std::cout << "After adding node5:" << std::endl;
    ch.addNode("node5");
    for (const auto& key : keys) {
        std::cout << "Key: " << key << " -> Node: " << ch.getNode(key) << std::endl;
    }
}

int main() {
    try {
        testConsistentHash();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
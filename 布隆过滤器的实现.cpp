#include <vector>
#include <string>
#include <functional>
#include <iostream>

using namespace std;
class BloomFilter {
private:
    std::vector<bool> bitArray;
    int size;
    int numHashes;

    // 简单的哈希函数
    size_t hash(const std::string& key, int seed) const {
        size_t result = 0;
        for (char c : key) {
            result = result * seed + c;
        }
        return result % size;
    }

public:
    BloomFilter(int size, int numHashes) : size(size), numHashes(numHashes) {
        bitArray.resize(size, false);
    }

    void add(const std::string& key) {
        for (int i = 0; i < numHashes; i++) {
            size_t index = hash(key, i + 1);
            bitArray[index] = true;
        }
    }

    bool mightContain(const std::string& key) const {
        for (int i = 0; i < numHashes; i++) {
            size_t index = hash(key, i + 1);
            if (!bitArray[index]) {
                return false;
            }
        }
        return true;
    }
};

// 使用示例
int main() {
    BloomFilter bf(1000, 3);  // 1000位的数组，使用3个哈希函数

    bf.add("apple");
    bf.add("banana");
    bf.add("orange");

    std::cout << "Contains apple? " << bf.mightContain("apple") << std::endl;
    std::cout << "Contains grape? " << bf.mightContain("grape") << std::endl;
    
    return 0;
}
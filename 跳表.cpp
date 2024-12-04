#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <limits>

template<typename K, typename V>
class SkipList {
private:
    struct Node {
        K key;
        V value;
        // 指向不同级别的下一个节点的指针数组
        Node** forward;
        int level;

        Node(K k, V v, int lvl) : key(k), value(v), level(lvl) {
            forward = new Node*[level + 1];
            for (int i = 0; i <= level; i++) {
                forward[i] = nullptr;
            }
        }

        ~Node() {
            delete[] forward;
        }
    };

    Node* head;      // 头节点
    int maxLevel;    // 最大层数
    int level;       // 当前层数
    float probability; // 层数增加的概率

    // 随机生成层数
    int randomLevel() {
        int lvl = 1;
        while ((float)rand()/RAND_MAX < probability && lvl < maxLevel) {
            lvl++;
        }
        return lvl;
    }

public:
    SkipList(int maxLvl = 16, float p = 0.5) 
        : maxLevel(maxLvl), level(0), probability(p) {
        srand(time(nullptr));
        K minKey = std::numeric_limits<K>::min();
        head = new Node(minKey, V(), maxLevel);
    }

    ~SkipList() {
        Node* current = head;
        while (current != nullptr) {
            Node* next = current->forward[0];
            delete current;
            current = next;
        }
    }

    // 插入键值对
    void insert(K key, V value) {
        Node* update[maxLevel + 1];
        Node* current = head;

        // 从最高层开始查找插入位置
        for (int i = level; i >= 0; i--) {
            while (current->forward[i] != nullptr && 
                   current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        current = current->forward[0];

        // 如果key已存在，更新value
        if (current != nullptr && current->key == key) {
            current->value = value;
            return;
        }

        // 生成随机层数
        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level + 1; i <= newLevel; i++) {
                update[i] = head;
            }
            level = newLevel;
        }

        // 创建新节点
        Node* newNode = new Node(key, value, newLevel);

        // 更新前向指针
        for (int i = 0; i <= newLevel; i++) {
            newNode->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = newNode;
        }
    }

    // 查找键对应的值
    bool search(K key, V& value) {
        Node* current = head;

        // 从最高层开始查找
        for (int i = level; i >= 0; i--) {
            while (current->forward[i] != nullptr && 
                   current->forward[i]->key < key) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];

        if (current != nullptr && current->key == key) {
            value = current->value;
            return true;
        }
        return false;
    }

    // 删除键值对
    bool remove(K key) {
        Node* update[maxLevel + 1];
        Node* current = head;

        // 从最高层开始查找
        for (int i = level; i >= 0; i--) {
            while (current->forward[i] != nullptr && 
                   current->forward[i]->key < key) {
                current = current->forward[i];
            }
            update[i] = current;
        }
        current = current->forward[0];

        // 如果找到key，删除节点
        if (current != nullptr && current->key == key) {
            for (int i = 0; i <= level; i++) {
                if (update[i]->forward[i] != current) {
                    break;
                }
                update[i]->forward[i] = current->forward[i];
            }

            // 更新level
            while (level > 0 && head->forward[level] == nullptr) {
                level--;
            }

            delete current;
            return true;
        }
        return false;
    }

    // 打印跳表
    void display() {
        for (int i = level; i >= 0; i--) {
            Node* current = head->forward[i];
            std::cout << "Level " << i << ": ";
            while (current != nullptr) {
                std::cout << current->key << ":" << current->value << " ";
                current = current->forward[i];
            }
            std::cout << std::endl;
        }
    }
};

int main() {
    // 创建跳表
    SkipList<int, std::string> skipList(4, 0.5);
    
    // 插入一些数据
    skipList.insert(3, "three");
    skipList.insert(6, "six");
    skipList.insert(7, "seven");
    skipList.insert(9, "nine");
    skipList.insert(12, "twelve");
    
    // 显示跳表
    std::cout << "Skip List content:" << std::endl;
    skipList.display();
    
    // 查找测试
    std::string value;
    if (skipList.search(6, value)) {
        std::cout << "Found 6, value = " << value << std::endl;
    }
    
    // 删除测试
    skipList.remove(6);
    std::cout << "\nAfter removing 6:" << std::endl;
    skipList.display();
    
    return 0;
}
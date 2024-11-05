#include <iostream>
#include <vector>
#include <cstddef>
#include <cassert>

class MemoryPool {
private:
    struct Block {
        Block* next;  // 指向下一个空闲块
    };

    size_t blockSize;      // 每个内存块的大小
    size_t poolSize;       // 内存池总大小
    char* memory;          // 内存池起始地址
    Block* freeList;       // 空闲块链表
    std::vector<char*> allocatedBlocks;  // 已分配的大块内存

public:
    // 构造函数：指定单个块大小和内存池中块的数量
    MemoryPool(size_t blockSize, size_t numBlocks) 
        : blockSize(std::max(blockSize, sizeof(Block))), 
          poolSize(numBlocks * blockSize), 
          memory(nullptr),
          freeList(nullptr) 
    {
        // 分配一大块内存
        allocatePool();
    }

    // 析构函数：释放所有内存
    ~MemoryPool() {
        // 释放所有分配的内存块
        for (char* block : allocatedBlocks) {
            delete[] block;
        }
    }

    // 分配一个内存块
    void* allocate() {
        if (freeList == nullptr) {
            // 如果没有空闲块，分配新的内存池
            allocatePool();
            if (freeList == nullptr) {
                return nullptr;  // 分配失败
            }
        }

        // 从空闲列表中获取一个块
        Block* block = freeList;
        freeList = block->next;
        return block;
    }

    // 释放一个内存块
    void deallocate(void* ptr) {
        if (ptr == nullptr) return;

        // 将释放的块添加到空闲列表的头部
        Block* block = static_cast<Block*>(ptr);
        block->next = freeList;
        freeList = block;
    }

    // 获取块大小
    size_t getBlockSize() const {
        return blockSize;
    }

private:
    // 分配新的内存池
    void allocatePool() {
        // 分配一大块内存
        char* newBlock = new char[poolSize];
        allocatedBlocks.push_back(newBlock);

        // 将内存池分割成多个块，并链接起来
        freeList = reinterpret_cast<Block*>(newBlock);
        Block* current = freeList;

        // 计算可以分配的块数
        size_t numBlocks = poolSize / blockSize;

        // 链接所有的空闲块
        for (size_t i = 0; i < numBlocks - 1; ++i) {
            current->next = reinterpret_cast<Block*>(newBlock + (i + 1) * blockSize);
            current = current->next;
        }
        current->next = nullptr;  // 最后一个块
    }
};

// 使用示例
class MyClass {
    int x, y;
public:
    MyClass(int a = 0, int b = 0) : x(a), y(b) {
        std::cout << "MyClass constructed: " << x << ", " << y << std::endl;
    }
    ~MyClass() {
        std::cout << "MyClass destructed: " << x << ", " << y << std::endl;
    }
};

int main() {
    // 创建一个内存池，每个块32字节，总共10个块
    MemoryPool pool(sizeof(MyClass), 10);

    // 用于存储分配的对象指针
    std::vector<MyClass*> objects;

    // 分配一些对象
    for (int i = 0; i < 5; ++i) {
        void* memory = pool.allocate();
        if (memory) {
            // 使用定位new在分配的内存上构造对象
            MyClass* obj = new(memory) MyClass(i, i+1);
            objects.push_back(obj);
        }
    }

    // 释放一些对象
    for (auto obj : objects) {
        // 调用析构函数
        obj->~MyClass();
        // 释放内存
        pool.deallocate(obj);
    }

    return 0;
}
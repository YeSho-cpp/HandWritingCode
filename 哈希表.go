package main

import (
	"fmt"
	"sync"
)

// 哈希表中的键值对
type entry[K comparable, V any] struct {
	key   K
	value V
	next  *entry[K, V] // 链表指针，用于处理冲突
}

// 哈希表结构
type HashTable[K comparable, V any] struct {
	buckets    []*entry[K, V] // 桶数组
	size       int            // 当前元素数量
	capacity   int            // 当前容量
	loadFactor float64        // 负载因子
	mu         sync.RWMutex   // 读写锁，用于并发安全
}

// 创建新的哈希表
func NewHashTable[K comparable, V any](initialCapacity int) *HashTable[K, V] {
	if initialCapacity < 1 {
		initialCapacity = 16 // 默认初始容量
	}
	return &HashTable[K, V]{
		buckets:    make([]*entry[K, V], initialCapacity),
		capacity:   initialCapacity,
		loadFactor: 0.75, // 默认负载因子
	}
}

// 计算哈希值
func (ht *HashTable[K, V]) hash(key K) int {
	// 使用Go内置的hash函数
	h := fmt.Sprintf("%v", key)
	hashValue := 0
	for i := 0; i < len(h); i++ {
		hashValue = 31*hashValue + int(h[i])
	}
	return abs(hashValue) % ht.capacity
}

// 插入或更新键值对
func (ht *HashTable[K, V]) Put(key K, value V) {
	ht.mu.Lock()
	defer ht.mu.Unlock()

	// 检查是否需要扩容
	if float64(ht.size+1)/float64(ht.capacity) >= ht.loadFactor {
		ht.resize(ht.capacity * 2)
	}

	index := ht.hash(key)
	// 如果bucket为空，直接插入
	if ht.buckets[index] == nil {
		ht.buckets[index] = &entry[K, V]{key: key, value: value}
		ht.size++
		return
	}

	// 遍历链表
	current := ht.buckets[index]
	// 如果key已存在，更新value
	if current.key == key {
		current.value = value
		return
	}

	// 处理哈希冲突：链表法
	for current.next != nil {
		if current.next.key == key {
			current.next.value = value
			return
		}
		current = current.next
	}

	// 在链表末尾添加新节点
	current.next = &entry[K, V]{key: key, value: value}
	ht.size++
}

// 获取值
func (ht *HashTable[K, V]) Get(key K) (V, bool) {
	ht.mu.RLock()
	defer ht.mu.RUnlock()

	index := ht.hash(key)
	current := ht.buckets[index]

	// 遍历链表查找
	for current != nil {
		if current.key == key {
			return current.value, true
		}
		current = current.next
	}

	// 如果未找到，返回零值
	var zero V
	return zero, false
}

// 删除键值对
func (ht *HashTable[K, V]) Delete(key K) bool {
	ht.mu.Lock()
	defer ht.mu.Unlock()

	index := ht.hash(key)
	current := ht.buckets[index]

	// 如果bucket为空
	if current == nil {
		return false
	}

	// 如果是第一个节点
	if current.key == key {
		ht.buckets[index] = current.next
		ht.size--
		return true
	}

	// 遍历链表查找
	for current.next != nil {
		if current.next.key == key {
			current.next = current.next.next
			ht.size--
			return true
		}
		current = current.next
	}

	return false
}

// 获取大小
func (ht *HashTable[K, V]) Size() int {
	ht.mu.RLock()
	defer ht.mu.RUnlock()
	return ht.size
}

// 扩容
func (ht *HashTable[K, V]) resize(newCapacity int) {
	oldBuckets := ht.buckets
	ht.buckets = make([]*entry[K, V], newCapacity)
	ht.capacity = newCapacity

	// 重新哈希所有现有元素
	for _, bucket := range oldBuckets {
		current := bucket
		for current != nil {
			index := ht.hash(current.key)
			// 将节点插入新bucket
			if ht.buckets[index] == nil {
				ht.buckets[index] = &entry[K, V]{key: current.key, value: current.value}
			} else {
				// 处理冲突
				newEntry := &entry[K, V]{key: current.key, value: current.value}
				newEntry.next = ht.buckets[index]
				ht.buckets[index] = newEntry
			}
			current = current.next
		}
	}
}

// 辅助函数：取绝对值
func abs(x int) int {
	if x < 0 {
		return -x
	}
	return x
}

// 示例用法
func main() {
	// 创建一个string->int的哈希表
	ht := NewHashTable[string, int](16)

	// 插入一些键值对
	ht.Put("one", 1)
	ht.Put("two", 2)
	ht.Put("three", 3)

	// 获取值
	if val, exists := ht.Get("two"); exists {
		fmt.Printf("Value for key 'two': %d\n", val)
	}

	// 更新值
	ht.Put("two", 22)
	if val, exists := ht.Get("two"); exists {
		fmt.Printf("Updated value for key 'two': %d\n", val)
	}

	// 删除键值对
	ht.Delete("one")
	fmt.Printf("Size after deletion: %d\n", ht.Size())

	// 尝试获取不存在的键
	if _, exists := ht.Get("one"); !exists {
		fmt.Println("Key 'one' no longer exists")
	}

	// 测试并发安全性
	var wg sync.WaitGroup
	for i := 0; i < 100; i++ {
		wg.Add(1)
		go func(i int) {
			defer wg.Done()
			key := fmt.Sprintf("key%d", i)
			ht.Put(key, i)
		}(i)
	}
	wg.Wait()

	fmt.Printf("Final size: %d\n", ht.Size())
}
package main

import (
    "sync"
)

type RWLock struct {
    mu      sync.Mutex
    cond    *sync.Cond
    readers int
    writers int
    waitingWriters int
}

func NewRWLock() *RWLock {
    rw := &RWLock{}
    rw.cond = sync.NewCond(&rw.mu)
    return rw
}

// 读锁
func (rw *RWLock) RLock() {
    rw.mu.Lock()
    // 等待直到没有写者和等待的写者
    for rw.writers > 0 || rw.waitingWriters > 0 {
        rw.cond.Wait()
    }
    rw.readers++
    rw.mu.Unlock()
}

// 读解锁
func (rw *RWLock) RUnlock() {
    rw.mu.Lock()
    rw.readers--
    // 如果是最后一个读者，通知所有等待的写者
    if rw.readers == 0 {
        rw.cond.Broadcast()
    }
    rw.mu.Unlock()
}

// 写锁
func (rw *RWLock) Lock() {
    rw.mu.Lock()
    rw.waitingWriters++
    // 等待直到没有读者和写者
    for rw.readers > 0 || rw.writers > 0 {
        rw.cond.Wait()
    }
    rw.waitingWriters--
    rw.writers++
    rw.mu.Unlock()
}

// 写解锁
func (rw *RWLock) Unlock() {
    rw.mu.Lock()
    rw.writers--
    // 通知所有等待的线程
    rw.cond.Broadcast()
    rw.mu.Unlock()
}

// 使用示例
func main() {
    rwlock := NewRWLock()
    
    // 读操作
    rwlock.RLock()
    // 读取共享资源
    rwlock.RUnlock()
    
    // 写操作
    rwlock.Lock()
    // 修改共享资源
    rwlock.Unlock()
}
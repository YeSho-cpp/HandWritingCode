#include <mutex>
#include <condition_variable>

class RWLock {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int readers = 0;        // 当前读者数量
    bool writing = false;   // 是否有写者
    int waitingWriters = 0; // 等待的写者数量

public:
    // 读锁
    void readLock() {
        std::unique_lock<std::mutex> lock(mtx);
        // 等待直到没有写者，并且没有等待的写者
        while (writing || waitingWriters > 0) {
            cv.wait(lock);
        }
        readers++;
    }

    // 读解锁
    void readUnlock() {
        std::unique_lock<std::mutex> lock(mtx);
        readers--;
        // 如果是最后一个读者，通知写者
        if (readers == 0) {
            cv.notify_all();
        }
    }

    // 写锁
    void writeLock() {
        std::unique_lock<std::mutex> lock(mtx);
        waitingWriters++;
        // 等待直到没有读者和写者
        while (readers > 0 || writing) {
            cv.wait(lock);
        }
        waitingWriters--;
        writing = true;
    }

    // 写解锁
    void writeUnlock() {
        std::unique_lock<std::mutex> lock(mtx);
        writing = false;
        // 通知所有等待的线程
        cv.notify_all();
    }
};

// 使用示例
void example() {
    RWLock rwlock;
    
    // 读操作
    rwlock.readLock();
    // 读取共享资源
    rwlock.readUnlock();
    
    // 写操作
    rwlock.writeLock();
    // 修改共享资源
    rwlock.writeUnlock();
}
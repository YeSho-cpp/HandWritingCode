#include <iostream>
#include <queue>
#include <chrono>
#include <thread>
#include <mutex>

class RateLimiter {
private:
    std::queue<int64_t> requests;  // 请求时间窗口
    int limit;                     // 每秒允许的请求数
    std::mutex mtx;                // 互斥锁
    const int64_t WINDOW_SIZE = 1000;  // 窗口大小(毫秒)

public:
    RateLimiter(int requestsPerSecond) : limit(requestsPerSecond) {}

    bool allowRequest() {
        std::lock_guard<std::mutex> lock(mtx);
        
        auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        // 移除超过1秒的请求
        while (!requests.empty() && requests.front() <= now - WINDOW_SIZE) {
            requests.pop();
        }

        // 检查是否超过限制
        if (requests.size() < limit) {
            requests.push(now);
            return true;
        }
        return false;
    }
};

// 模拟处理请求的函数
void processRequest(int requestId) {
    // 模拟处理请求的耗时
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "处理请求 " << requestId << std::endl;
}

int main() {
    RateLimiter limiter(5);  // 每秒允许5个请求
    
    // 模拟10个并发请求
    for (int i = 0; i < 10; i++) {
        std::thread([&limiter, i]() {
            if (limiter.allowRequest()) {
                processRequest(i);
            } else {
                std::cout << "请求 " << i << " 被限流" << std::endl;
            }
        }).detach();
        
        // 模拟请求间隔
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // 等待所有请求处理完成
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class Timer {
public:
  // 定时任务结构体
  struct TimerTask {
    int id;                                           // 任务ID
    std::chrono::steady_clock::time_point expireTime; // 到期时间
    std::function<void()> callback;                   // 回调函数

    // 比较运算符重载，用于小顶堆
    bool operator>(const TimerTask &other) const {
      return expireTime > other.expireTime;
    }
  };

private:
  // 使用优先队列（小顶堆）存储定时任务
  std::priority_queue<TimerTask, std::vector<TimerTask>,
                      std::greater<TimerTask>>
      timerQueue;
  std::mutex mutex;
  std::condition_variable cv;
  std::atomic<bool> running{true};
  std::thread timerThread;
  int nextTaskId{0};

public:
  Timer() {
    // 启动定时器线程
    timerThread = std::thread([this]() { this->processTimerTasks(); });
  }

  ~Timer() {
    // 停止定时器线程
    running = false;
    cv.notify_one();
    if (timerThread.joinable()) {
      timerThread.join();
    }
  }

  // 添加定时任务（延迟执行）
  int addTimer(int delayMs, std::function<void()> callback) {
    std::lock_guard<std::mutex> lock(mutex);
    int taskId = nextTaskId++;

    auto expireTime =
        std::chrono::steady_clock::now() + std::chrono::milliseconds(delayMs);

    timerQueue.push({taskId, expireTime, std::move(callback)});
    cv.notify_one(); // 通知处理线程
    return taskId;
  }

  // 添加周期性定时任务
  void addRepeatingTimer(int intervalMs, std::function<void()> callback) {
    std::function<void()> repeatingCallback;
    repeatingCallback = [=]() {
      callback();                              // 执行回调
      addTimer(intervalMs, repeatingCallback); // 添加下一次执行
    };

    addTimer(intervalMs, repeatingCallback);
  }

private:
  // 定时器处理线程的主循环
  void processTimerTasks() {
    while (running) {
      std::unique_lock<std::mutex> lock(mutex);

      if (timerQueue.empty()) {
        // 如果没有任务，等待新任务
        cv.wait(lock);
        continue;
      }

      auto &nextTask = timerQueue.top();
      auto now = std::chrono::steady_clock::now();

      if (nextTask.expireTime <= now) {
        // 任务到期，执行回调
        auto callback = std::move(nextTask.callback);
        timerQueue.pop();
        lock.unlock();

        callback(); // 执行回调函数
      } else {
        // 等待到下一个任务的时间
        cv.wait_until(lock, nextTask.expireTime);
      }
    }
  }
};

// 使用示例
int main() {
  Timer timer;

  // 添加一次性定时器
  std::cout << "添加一次性定时器（2秒后执行）" << std::endl;
  timer.addTimer(2000,
                 []() { std::cout << "一次性定时器触发！" << std::endl; });

  // 添加周期性定时器
  std::cout << "添加周期性定时器（每1秒执行一次）" << std::endl;
  timer.addRepeatingTimer(
      1000, []() { std::cout << "周期性定时器触发！" << std::endl; });

  // 等待5秒后退出
  std::this_thread::sleep_for(std::chrono::seconds(5));
  std::cout << "程序退出" << std::endl;

  return 0;
}
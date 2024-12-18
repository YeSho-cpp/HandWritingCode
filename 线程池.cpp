#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <numeric>

class ThreadPool {
public:
    ThreadPool(size_t threads);
    ~ThreadPool();
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::invoke_result_t<F, Args...>>;  // 注意这里
    
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};

ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for(size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this] {
            for(;;) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if(this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for(std::thread &worker : workers) {
        worker.join();
    }
}

template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args) 
    -> std::future<typename std::invoke_result<F, Args...>::type> 
{
    using return_type = typename std::invoke_result<F, Args...>::type;
    
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );
    
    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        
        if(stop) throw std::runtime_error("enqueue on stopped ThreadPool");
        
        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

// 添加并行计算功能
class ParallelComputer {
private:
    ThreadPool& pool;
    size_t num_threads;

public:
    ParallelComputer(ThreadPool& p, size_t threads) : pool(p), num_threads(threads) {}

    // 并行计算数组和
    int64_t parallelSum(const std::vector<int>& arr) {
        if (arr.empty()) return 0;

        size_t chunk_size = (arr.size() + num_threads - 1) / num_threads;
        std::vector<std::future<int64_t>> futures;

        // 将数组分成多个块并分配给线程池
        for (size_t i = 0; i < arr.size(); i += chunk_size) {
            size_t end = std::min(i + chunk_size, arr.size());
            auto future = pool.enqueue([&arr, i, end]() -> int64_t {
                return std::accumulate(arr.begin() + i, arr.begin() + end, 0LL);
            });
            futures.push_back(std::move(future));
        }

        // 收集并合并结果
        int64_t total = 0;
        for (auto& future : futures) {
            total += future.get();
        }
        return total;
    }

    // 并行查找数组中的最大值
    int parallelMax(const std::vector<int>& arr) {
        if (arr.empty()) throw std::runtime_error("Empty array");

        size_t chunk_size = (arr.size() + num_threads - 1) / num_threads;
        std::vector<std::future<int>> futures;

        for (size_t i = 0; i < arr.size(); i += chunk_size) {
            size_t end = std::min(i + chunk_size, arr.size());
            auto future = pool.enqueue([&arr, i, end]() -> int {
                return *std::max_element(arr.begin() + i, arr.begin() + end);
            });
            futures.push_back(std::move(future));
        }

        int max_value = futures[0].get();
        for (size_t i = 1; i < futures.size(); ++i) {
            max_value = std::max(max_value, futures[i].get());
        }
        return max_value;
    }

    // 并行统计数组中某个元素的出现次数
    int parallelCount(const std::vector<int>& arr, int target) {
        size_t chunk_size = (arr.size() + num_threads - 1) / num_threads;
        std::vector<std::future<int>> futures;

        for (size_t i = 0; i < arr.size(); i += chunk_size) {
            size_t end = std::min(i + chunk_size, arr.size());
            auto future = pool.enqueue([&arr, i, end, target]() -> int {
                return std::count(arr.begin() + i, arr.begin() + end, target);
            });
            futures.push_back(std::move(future));
        }

        int total_count = 0;
        for (auto& future : futures) {
            total_count += future.get();
        }
        return total_count;
    }
};


int main() {
    const size_t THREAD_COUNT = 4;
    ThreadPool pool(THREAD_COUNT);
    ParallelComputer computer(pool, THREAD_COUNT);

    // 创建测试数据
    std::vector<int> test_array(1000000);
    for (int i = 0; i < test_array.size(); ++i) {
        test_array[i] = i % 100;  // 生成0-99的循环数
    }

    // 测试并行求和
    auto start_time = std::chrono::high_resolution_clock::now();
    int64_t sum = computer.parallelSum(test_array);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Parallel sum: " << sum << std::endl;
    std::cout << "Time taken: " << duration.count() << "ms" << std::endl;

    // 测试并行查找最大值
    start_time = std::chrono::high_resolution_clock::now();
    int max_value = computer.parallelMax(test_array);
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Maximum value: " << max_value << std::endl;
    std::cout << "Time taken: " << duration.count() << "ms" << std::endl;

    // 测试并行计数
    int target = 42;
    start_time = std::chrono::high_resolution_clock::now();
    int count = computer.parallelCount(test_array, target);
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Count of " << target << ": " << count << std::endl;
    std::cout << "Time taken: " << duration.count() << "ms" << std::endl;

    // 对比单线程性能
    start_time = std::chrono::high_resolution_clock::now();
    int64_t single_thread_sum = std::accumulate(test_array.begin(), test_array.end(), 0LL);
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "\nSingle thread sum: " << single_thread_sum << std::endl;
    std::cout << "Time taken: " << duration.count() << "ms" << std::endl;

    return 0;
}
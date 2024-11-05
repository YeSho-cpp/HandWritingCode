package main

import (
    "fmt"
    "sync"
    "time"
)

// RateLimiter 简单的计数器限流器
type RateLimiter struct {
    rate  int           // 每秒允许的请求数
    mu    sync.Mutex    // 互斥锁
    count int           // 当前时间窗口内的请求数
    reset time.Time     // 重置时间
}

// NewRateLimiter 创建一个新的限流器
func NewRateLimiter(rate int) *RateLimiter {
    return &RateLimiter{
        rate:  rate,
        reset: time.Now(),
    }
}

// Allow 判断请求是否允许通过
func (rl *RateLimiter) Allow() bool {
    rl.mu.Lock()
    defer rl.mu.Unlock()

    now := time.Now()
    
    // 如果已经过了1秒，重置计数器
    if now.Sub(rl.reset) >= time.Second {
        rl.count = 0
        rl.reset = now
    }

    // 如果当前计数小于速率限制，允许请求
    if rl.count < rl.rate {
        rl.count++
        return true
    }
    
    return false
}

func main() {
    // 创建限流器，每秒允许5个请求
    limiter := NewRateLimiter(5)
    
    // 模拟10个并发请求
    var wg sync.WaitGroup
    for i := 0; i < 10; i++ {
        wg.Add(1)
        go func(id int) {
            defer wg.Done()
            if limiter.Allow() {
                fmt.Printf("请求 %d 通过\n", id)
            } else {
                fmt.Printf("请求 %d 被限流\n", id)
            }
        }(i)
        // 每隔100ms发送一个请求
        time.Sleep(100 * time.Millisecond)
    }
    
    wg.Wait()
}
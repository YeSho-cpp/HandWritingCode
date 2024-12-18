package limiter

import (
    "context"
    "sync"
    "time"
	"fmt"
)

// todo 难点1 怎么以一定的速率放入令牌，开个线程背后去做，

// func generateTokens(){
//     ticker:=time.NewTicker(time.Millisecond*100)
//     defer ticker.Stop()
//     for{
        
//         select {
//         case <-ticker.C:
//             // 加锁计算比之前流逝的时间需要放多少令牌
//         case :// 结果表示
//             return
//         }

//     }
// }

// TokenBucket 令牌桶限流器
type TokenBucket struct {
    rate       float64 // 令牌产生速率(个/秒)
    capacity   float64 // 桶容量
    tokens     float64 // 当前令牌数
    lastUpdate time.Time // 上次更新时间
    mutex      sync.Mutex
    stop       chan struct{} // 用于停止令牌产生
}

// NewTokenBucket 创建一个令牌桶限流器
func NewTokenBucket(rate, capacity float64) *TokenBucket {
    tb := &TokenBucket{
        rate:       rate,
        capacity:   capacity,
        tokens:     capacity,
        lastUpdate: time.Now(),
        stop:       make(chan struct{}),
    }
    // 启动令牌生成器
    go tb.generateTokens()
    return tb
}

// generateTokens 持续生成令牌
func (tb *TokenBucket) generateTokens() {
    ticker := time.NewTicker(time.Millisecond * 100) // 每100ms更新一次
    defer ticker.Stop()

    for {
        select {
        case <-ticker.C:
            tb.mutex.Lock()
            now := time.Now()
            elapsed := now.Sub(tb.lastUpdate).Seconds()
            // 计算新产生的令牌数
            newTokens := elapsed * tb.rate
            // 更新令牌数，不超过容量
            tb.tokens = min(tb.capacity, tb.tokens+newTokens)
            tb.lastUpdate = now
            tb.mutex.Unlock()
        case <-tb.stop:
            return
        }
    }
}

// Allow 判断是否允许通过
func (tb *TokenBucket) Allow() bool {
    return tb.AllowN(1)
}

// AllowN 判断是否允许n个请求通过
func (tb *TokenBucket) AllowN(n float64) bool {
    tb.mutex.Lock()
    defer tb.mutex.Unlock()

    if tb.tokens >= n {
        tb.tokens -= n
        return true
    }
    return false
}

// Wait 等待直到获取到令牌
func (tb *TokenBucket) Wait(ctx context.Context) error {
    return tb.WaitN(ctx, 1)
}

// WaitN 等待直到获取到n个令牌
func (tb *TokenBucket) WaitN(ctx context.Context, n float64) error {
    ticker := time.NewTicker(time.Millisecond * 10)
    defer ticker.Stop()

    for {
        select {
        case <-ctx.Done():
            return ctx.Err()
        case <-ticker.C:
            if tb.AllowN(n) {
                return nil
            }
        }
    }
}

// Close 关闭令牌桶
func (tb *TokenBucket) Close() {
    close(tb.stop)
}

// 辅助函数
func min(a, b float64) float64 {
    if a < b {
        return a
    }
    return b
}


func Example() {
	// 创建一个限流器：每秒产生10个令牌，最大容量为20
	limiter := NewTokenBucket(10, 20)
	defer limiter.Close()

	// 1. 简单使用
	if limiter.Allow() {
			// 处理请求
			fmt.Println("请求被允许")
	} else {
			fmt.Println("请求被限流")
	}

	// 2. 批量请求
	if limiter.AllowN(5) {
			fmt.Println("允许5个请求")
	}

	// 3. 等待方式
	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	if err := limiter.Wait(ctx); err != nil {
			fmt.Println("等待超时:", err)
	} else {
			fmt.Println("成功获取令牌")
	}
}
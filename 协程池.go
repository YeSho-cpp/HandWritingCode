package pool

import (
    "context"
    "errors"
    "fmt"
    "sync"
    "sync/atomic"
    "time"
)

// Task 定义任务接口
type Task interface {
    Execute() error
}

// Pool 协程池结构体
type Pool struct {
    // 核心配置
    capacity    int32         // 池容量
    active     int32         // 当前活跃的协程数
    workers    chan struct{} // 控制协程数量的通道
    tasks      chan Task     // 任务队列

    // 控制相关
    ctx        context.Context
    cancel     context.CancelFunc
    wg         sync.WaitGroup

    // 监控指标
    completed  int64         // 完成的任务数
    failed     int64         // 失败的任务数
    
    // 配置选项
    panicHandler func(interface{})
    timeout      time.Duration
}

// Option 定义配置选项函数类型
type Option func(*Pool)

// NewPool 创建协程池
func NewPool(capacity int32, options ...Option) *Pool {
    ctx, cancel := context.WithCancel(context.Background())
    
    p := &Pool{
        capacity:     capacity,
        workers:      make(chan struct{}, capacity),
        tasks:        make(chan Task, capacity*2),
        ctx:         ctx,
        cancel:      cancel,
        timeout:     30 * time.Second,
        panicHandler: defaultPanicHandler,
    }

    // 应用配置选项
    for _, opt := range options {
        opt(p)
    }

    // 启动监控
    go p.monitor()

    return p
}

// WithTimeout 设置任务超时时间
func WithTimeout(timeout time.Duration) Option {
    return func(p *Pool) {
        p.timeout = timeout
    }
}

// WithPanicHandler 设置panic处理函数
func WithPanicHandler(handler func(interface{})) Option {
    return func(p *Pool) {
        p.panicHandler = handler
    }
}

// Submit 提交任务
func (p *Pool) Submit(task Task) error {
    select {
    case <-p.ctx.Done():
        return errors.New("pool is closed")
    case p.tasks <- task:
        // 如果当前活跃协程数小于容量，则创建新的协程
        if atomic.LoadInt32(&p.active) < p.capacity {
            p.addWorker()
        }
        return nil
    default:
        return errors.New("task queue is full")
    }
}

// addWorker 添加工作协程
func (p *Pool) addWorker() {
    select {
    case p.workers <- struct{}{}:
        atomic.AddInt32(&p.active, 1)
        p.wg.Add(1)
        
        go func() {
            defer func() {
                if r := recover(); r != nil {
                    p.panicHandler(r)
                }
                <-p.workers
                atomic.AddInt32(&p.active, -1)
                p.wg.Done()
            }()

            p.work()
        }()
    default:
        // workers channel已满，说明达到容量上限
    }
}

// work 工作协程的主循环
func (p *Pool) work() {
    for {
        select {
        case <-p.ctx.Done():
            return
        case task, ok := <-p.tasks:
            if !ok {
                return
            }
            
            // 执行任务
            ctx, cancel := context.WithTimeout(p.ctx, p.timeout)
            done := make(chan error)
            
            go func() {
                done <- task.Execute()
            }()

            select {
            case err := <-done:
                if err != nil {
                    atomic.AddInt64(&p.failed, 1)
                } else {
                    atomic.AddInt64(&p.completed, 1)
                }
            case <-ctx.Done():
                atomic.AddInt64(&p.failed, 1)
            }
            
            cancel()
        }
    }
}

// Stats 返回池统计信息
type Stats struct {
    Capacity   int32
    Active     int32
    Completed  int64
    Failed     int64
}

// GetStats 获取统计信息
func (p *Pool) GetStats() Stats {
    return Stats{
        Capacity:  p.capacity,
        Active:    atomic.LoadInt32(&p.active),
        Completed: atomic.LoadInt64(&p.completed),
        Failed:    atomic.LoadInt64(&p.failed),
    }
}

// monitor 监控协程池状态
func (p *Pool) monitor() {
    ticker := time.NewTicker(time.Second)
    defer ticker.Stop()

    for {
        select {
        case <-p.ctx.Done():
            return
        case <-ticker.C:
            stats := p.GetStats()
            fmt.Printf("Pool Stats - Active: %d, Completed: %d, Failed: %d\n",
                stats.Active, stats.Completed, stats.Failed)
        }
    }
}

// Close 关闭协程池
func (p *Pool) Close() {
    p.cancel()
    close(p.tasks)
    p.wg.Wait()
}

// defaultPanicHandler 默认的panic处理函数
func defaultPanicHandler(r interface{}) {
    fmt.Printf("Worker panic: %v\n", r)
}
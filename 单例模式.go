package main

import (
    "sync"
)

type Singleton struct {
    // 单例的属性和方法
}

var (
    instance *Singleton
    once     sync.Once
)

func GetInstance() *Singleton {
    once.Do(func() {
        instance = &Singleton{}
    })
    return instance
}
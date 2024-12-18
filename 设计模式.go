package main

import (
	"fmt"
	"sync"
)

// 1.单例模式

type Singleton struct {
	data string
	end  chan int
}

var (
	instance *Singleton
	once     sync.Once
)

func GetInstance() *Singleton {
	once.Do(func() {
		instance = &Singleton{
			data: "hello world",
		}
	})
	return instance
}

// 2.工厂模式

type Smile interface {
	Transform()
	Ability()
}

type SheepSmile struct{}

func (s *SheepSmile) Transform() {
	fmt.Println("变成人兽---山羊形态")
}

func (s *SheepSmile) Ability() {
	fmt.Println("将手臂变成绵羊角的招式--- 巨羊角")
}

type LionSmile struct{}

func (l *LionSmile) Transform() {
	fmt.Println("变成人兽 -- 狮子人形态...")
}

func (l *LionSmile) Ability() {
	fmt.Println("火遁· 豪火球之术...")
}

type SmileType int

const (
	SheepType SmileType = iota
	LionType
)

type SimpleSmileFactory struct{}

func (f *SimpleSmileFactory) CreateSmile(smileType SmileType) Smile {
	switch smileType {
	case SheepType:
		return &SheepSmile{}
	case LionType:
		return &LionSmile{}
	default:
		return nil
	}
}

// 3. 代理模式

type subject interface {
	Do() string
}

// 真实主题
type RealSubject struct{}

func (r *RealSubject) Do() string {
	return "RealSubject doing something"
}

// 代理
type Proxy struct {
	real *RealSubject
}

func NewProxy() *Proxy {
	return &Proxy{real: &RealSubject{}}
}

func (p *Proxy) Do() string {
	// 前置处理
	fmt.Println("Before doing something")

	// 调用真实对象
	result := p.real.Do()

	// 后置处理
	fmt.Println("After doing something")
	return result
}

// 4. 装饰器模式

type Coffee interface {
	Cost() float64
	Description() string
}

// SimpleCoffee 具体的组件
type SimpleCoffee struct{}

func (c *SimpleCoffee) Cost() float64 {
	return 10.0
}

func (c *SimpleCoffee) Description() string {
	return "Simple Coffee"
}

// CoffeeDecorator 装饰器基础结构
type CoffeeDecorator struct {
	coffee Coffee
}

func (c *CoffeeDecorator) Cost() float64 {
	return c.coffee.Cost()
}

func (c *CoffeeDecorator) Description() string {
	return c.coffee.Description()
}

// MilkDecorator 具体的牛奶装饰器
type MilkDecorator struct {
	CoffeeDecorator
}

func NewMilkDecorator(coffee Coffee) *MilkDecorator {
	return &MilkDecorator{CoffeeDecorator{coffee}}
}

func (m *MilkDecorator) Cost() float64 {
	return m.coffee.Cost() + 2.0
}

func (m *MilkDecorator) Description() string {
	return m.coffee.Description() + " + Milk"
}

// SugarDecorator 具体的糖装饰器
type SugarDecorator struct {
	CoffeeDecorator
}

func NewSugarDecorator(coffee Coffee) *SugarDecorator {
	return &SugarDecorator{CoffeeDecorator{coffee}}
}

func (s *SugarDecorator) Cost() float64 {
	return s.coffee.Cost() + 1.0
}

func (s *SugarDecorator) Description() string {
	return s.coffee.Description() + " + Sugar"
}

func testDecorator() {
	// 创建一个基础咖啡
	coffee := &SimpleCoffee{}

	fmt.Printf("%s costs $%.2f\n", coffee.Description(), coffee.Cost())

	// 加牛奶
	coffeeWithMilk := NewMilkDecorator(coffee)
	fmt.Printf("%s costs $%.2f\n", coffeeWithMilk.Description(), coffeeWithMilk.Cost())

	// 加牛奶和糖
	coffeeWithMilkAndSugar := NewSugarDecorator(NewMilkDecorator(coffee))
	fmt.Printf("%s costs $%.2f\n", coffeeWithMilkAndSugar.Description(), coffeeWithMilkAndSugar.Cost())

	// 只加糖
	coffeeWithSugar := NewSugarDecorator(coffee)
	fmt.Printf("%s costs $%.2f\n", coffeeWithSugar.Description(), coffeeWithSugar.Cost())
}

// 5. 观察者模式

// Observer 定义观察者接口
type Observer interface {
	Update(message string)
}

// Subject 定义主题接口
type Subject interface {
	Attach(observer Observer)
	Detach(observer Observer)
	Notify(message string)
}

// NewsAgency 具体主题
type NewsAgency struct {
	observers []Observer
}

// NewNewsAgency 创建新的新闻机构
func NewNewsAgency() *NewsAgency {
	return &NewsAgency{
		observers: make([]Observer, 0),
	}
}

// Attach 添加观察者
func (n *NewsAgency) Attach(observer Observer) {
	n.observers = append(n.observers, observer)
}

// Detach 移除观察者
func (n *NewsAgency) Detach(observer Observer) {
	for i, obs := range n.observers {
		if obs == observer {
			n.observers = append(n.observers[:i], n.observers[i+1:]...)
			break
		}
	}
}

// Notify 通知所有观察者
func (n *NewsAgency) Notify(message string) {
	for _, observer := range n.observers {
		observer.Update(message)
	}
}

// PublishNews 发布新闻
func (n *NewsAgency) PublishNews(news string) {
	fmt.Printf("\nNewsAgency: Publishing news: %s\n", news)
	n.Notify(news)
}

// NewsSubscriber 具体观察者
type NewsSubscriber struct {
	name string
}

// NewNewsSubscriber 创建新的订阅者
func NewNewsSubscriber(name string) *NewsSubscriber {
	return &NewsSubscriber{
		name: name,
	}
}

// Update 实现Observer接口
func (s *NewsSubscriber) Update(message string) {
	fmt.Printf("Subscriber %s received news: %s\n", s.name, message)
}

// 6. 责任链模式

// Request 请求结构体
type Request struct {
	RequestType string
	Amount      int
}

// Handler 处理者接口
type Handler interface {
	Handle(request Request)
	SetNext(handler Handler)
}

// BaseHandler 基础处理者结构体
type BaseHandler struct {
	name       string
	next       Handler
	canHandle  func(request Request) bool
	handleFunc func(request Request)
}

// Handle 处理请求
func (h *BaseHandler) Handle(request Request) {
	if h.canHandle(request) {
		h.handleFunc(request)
	} else if h.next != nil {
		fmt.Printf("%s forwards request to next handler\n", h.name)
		h.next.Handle(request)
	} else {
		fmt.Printf("Request cannot be handled\n")
	}
}

// SetNext 设置下一个处理者
func (h *BaseHandler) SetNext(handler Handler) {
	h.next = handler
}

// NewTeamLeader 创建团队领导
func NewTeamLeader() Handler {
	return &BaseHandler{
		name: "TeamLeader",
		canHandle: func(request Request) bool {
			return request.RequestType == "leave" && request.Amount <= 1
		},
		handleFunc: func(request Request) {
			fmt.Printf("TeamLeader approves %d day leave request\n", request.Amount)
		},
	}
}

// NewManager 创建经理
func NewManager() Handler {
	return &BaseHandler{
		name: "Manager",
		canHandle: func(request Request) bool {
			return (request.RequestType == "leave" && request.Amount <= 3) ||
				(request.RequestType == "expense" && request.Amount <= 1000)
		},
		handleFunc: func(request Request) {
			if request.RequestType == "leave" {
				fmt.Printf("Manager approves %d days leave request\n", request.Amount)
			} else {
				fmt.Printf("Manager approves $%d expense request\n", request.Amount)
			}
		},
	}
}

// NewDirector 创建总监
func NewDirector() Handler {
	return &BaseHandler{
		name: "Director",
		canHandle: func(request Request) bool {
			return (request.RequestType == "leave" && request.Amount <= 5) ||
				(request.RequestType == "expense" && request.Amount <= 5000)
		},
		handleFunc: func(request Request) {
			if request.RequestType == "leave" {
				fmt.Printf("Director approves %d days leave request\n", request.Amount)
			} else {
				fmt.Printf("Director approves $%d expense request\n", request.Amount)
			}
		},
	}
}

// NewCEO 创建CEO
func NewCEO() Handler {
	return &BaseHandler{
		name: "CEO",
		canHandle: func(request Request) bool {
			return (request.RequestType == "leave" && request.Amount <= 10) ||
				(request.RequestType == "expense" && request.Amount <= 10000)
		},
		handleFunc: func(request Request) {
			if request.RequestType == "leave" {
				fmt.Printf("CEO approves %d days leave request\n", request.Amount)
			} else {
				fmt.Printf("CEO approves $%d expense request\n", request.Amount)
			}
		},
	}
}

func Chain_of_responsibility() {
	// 创建处理者链
	teamLeader := NewTeamLeader()
	manager := NewManager()
	director := NewDirector()
	ceo := NewCEO()

	// 设置处理者链
	teamLeader.SetNext(manager)
	manager.SetNext(director)
	director.SetNext(ceo)

	// 测试请假请求
	fmt.Println("\nTesting leave requests:")
	requests := []Request{
		{"leave", 1},  // 应该由TeamLeader处理
		{"leave", 3},  // 应该由Manager处理
		{"leave", 5},  // 应该由Director处理
		{"leave", 7},  // 应该由CEO处理
		{"leave", 12}, // 应该被拒绝
	}

	for _, request := range requests {
		fmt.Printf("\nProcessing leave request for %d days:\n", request.Amount)
		teamLeader.Handle(request)
	}
}

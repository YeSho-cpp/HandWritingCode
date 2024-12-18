
#include <bits/stdc++.h>

using namespace std;

// 1. 单例模式
// 1.1 懒汉模式 在类加载的时候立刻进行实例化

#if 0
template<typename T>
class Singleton {
private:
    Singleton() = default;
   

    Singleton(const Singleton<T>&) = delete;
    Singleton& operator=(const Singleton<T>&) = delete;

    static std::shared_ptr<T> _instance;

public:
    ~Singleton() = default;
    static std::shared_ptr<T> GetInstance() {
        return _instance;
    }
};

#endif

// 1.2 懒汉模式
// 在类加载的时候不去创建这个唯一的实例，而是在需要使用的时候再进行实例化

#if 0

template <typename T> class Singleton {
private:
  Singleton() = default;

  Singleton(const Singleton<T> &) = delete;
  Singleton &operator=(const Singleton<T> &) = delete;

  static std::shared_ptr<T> _instance;

public:
  ~Singleton()=default;

  static std::shared_ptr<T> GetInstance(){
    static std::once_flag s_flag;
    std::call_once(s_flag,[](){
      _instance=make_shared<T>();
    });

    return _instance;
  }
};

#endif

// 1.3 局部变量
//

#if 1

template <typename T> class Singleton {
private:
  Singleton() = default;

  Singleton(const Singleton<T> &) = delete;
  Singleton &operator=(const Singleton<T> &) = delete;

public:
  ~Singleton() = default;

  static std::shared_ptr<T> GetInstance() {
    static std::shared_ptr<T> _instance = std::make_shared<T>();
    return _instance;
  }
};

#endif

// 2.工厂模式

// 2.1 简单工厂模式

#if 0
class Smile{ // 人造恶魔果实
public:
  virtual void transform()=0;
  virtual void ability()=0;
};

class SheepSmile:public Smile{ // 人造恶魔果实* 绵羊形态
public:
  void transform() override{
    cout<<"变成人兽---山羊形态"<<endl;
  }
  void ability() override{
    cout<<"将手臂变成绵羊角的招式--- 巨羊角"<<endl;
  }
};


class LionSmile:public Smile // 人造恶魔果实· 狮子形态
{
public:
    void transform() override
    {
        cout << "变成人兽 -- 狮子人形态..." << endl;
    }
    void ability() override
    {
        cout << "火遁· 豪火球之术..." << endl;
    }
};


class BatSmile:public Smile // 人造恶魔果实· 蝙蝠形态
{
public:
    void transform() override
    {
        cout << "变成人兽 -- 蝙蝠人形态..." << endl;
    }
    void ability() override
    {
        cout << "声纳引箭之万剑归宗..." << endl;
    }
};

enum class Type:char{SHEEP, LION, BAT};

// 简单工厂
class SmileFactory{
public:
  static std::unique_ptr<Smile>createSmile(Type type){
    switch (type) {
    case Type::SHEEP:
      return std::make_unique<SheepSmile>();
      break;
    case Type::LION:
      return std::make_unique<LionSmile>();
    case Type::BAT:
      return std::make_unique<BatSmile>();
    default:
      return nullptr;
    }
  }
};

#endif

// 2.2工厂方法模式

#if 0

class Smile {
public:
  virtual void transform() = 0;
  virtual void ability() = 0;
  virtual ~Smile() = default;
};

// 具体产品类
class SheepSmile : public Smile {
public:
    void transform() override {
        cout << "变成人兽---山羊形态" << endl;
    }
    void ability() override {
        cout << "将手臂变成绵羊角的招式--- 巨羊角" << endl;
    }
};

class LionSmile : public Smile {
public:
    void transform() override {
        cout << "变成人兽 -- 狮子人形态..." << endl;
    }
    void ability() override {
        cout << "火遁· 豪火球之术..." << endl;
    }
};

class BatSmile : public Smile {
public:
    void transform() override {
        cout << "变成人兽 -- 蝙蝠人形态..." << endl;
    }
    void ability() override {
        cout << "声纳引箭之万剑归宗..." << endl;
    }
};

// 工厂基类
class SmileFactory{
public:
  virtual std::unique_ptr<Smile>createSmile()=0;
  virtual ~SmileFactory()=default;
};


class SheepSmileFactory:public SmileFactory{
public:
  std::unique_ptr<Smile> createSmile() override{
    return std::make_unique<SheepSmile>();
  }
};

class LionSmileFactory : public SmileFactory {
public:
    std::unique_ptr<Smile> createSmile() override {
        return std::make_unique<LionSmile>();
    }
};

class BatSmileFactory : public SmileFactory {
public:
    std::unique_ptr<Smile> createSmile() override {
        return std::make_unique<BatSmile>();
    }
};

#endif

// 2.3抽象工厂模式

// 产品族A：人造恶魔果实形态
class SmileForm {
public:
  virtual void transform() = 0;
  virtual ~SmileForm() = 0;
};

class SheepForm : public SmileForm {
public:
  void transform() override { cout << "变成山羊形态" << endl; }
};

class LionForm : public SmileForm {
public:
  void transform() override { cout << "变成狮子形态" << endl; }
};

// 产品族B：恶魔果实能力
class SmileAbility {
public:
  virtual void useAbility() = 0;
  virtual ~SmileAbility() = default;
};

class SheepAbility : public SmileAbility {
public:
  void useAbility() override { cout << "使用巨羊角攻击" << endl; }
};

class LionAbility : public SmileAbility {
public:
  void useAbility() override { cout << "使用狮子咆哮" << endl; }
};

class AbstractSmileFactory {
public:
  virtual std::unique_ptr<SmileForm> createForm() = 0;
  virtual std::unique_ptr<SmileAbility> createAbility() = 0;
  virtual ~AbstractSmileFactory();
};

class SheepSmileCompleteFactory : public AbstractSmileFactory {
public:
  std::unique_ptr<SmileForm> createForm() override {
    return std::make_unique<SheepForm>();
  }
  std::unique_ptr<SmileAbility> createAbility() override {
    return std::make_unique<SheepAbility>();
  }
};

class LionSmileCompleteFactory : public AbstractSmileFactory {
public:
  std::unique_ptr<SmileForm> createForm() override {
    return std::make_unique<LionForm>();
  }
  std::unique_ptr<SmileAbility> createAbility() override {
    return std::make_unique<LionAbility>();
  }
};

// 3. 代理模式

class ISubject {
public:
  virtual void request() = 0;
  virtual ~ISubject() = default;
};

class RealSubject : public ISubject {
public:
  void request() override {
    std::cout << "RealSubject handling request" << std::endl;
  }
};

// 代理类

class Proxy : public ISubject {
private:
  std::unique_ptr<RealSubject> realSubject;

  void beforeRequest() {
    std::cout << "Proxy: Before handling request" << std::endl;
  }

  void afterRequest() {
    std::cout << "Proxy: After handling request" << std::endl;
  }

public:
  Proxy() : realSubject(std::make_unique<RealSubject>()) {}

  void request() {
    beforeRequest();
    realSubject->request();
    afterRequest();
  }
};

// 4. 装饰器模式
class Coffee {
public:
  virtual double cost() = 0;
  virtual std::string description() = 0;
  virtual ~Coffee() = default;
};

// 基础咖啡类

class SimpleCoffee : public Coffee {
public:
  double cost() override { return 10.0; }

  std::string description() override { return "Simle Coffee"; }
};

class CoffeeDecorator : public Coffee {
private:
  std::unique_ptr<Coffee> coffee;

public:
  explicit CoffeeDecorator(std::unique_ptr<Coffee> c) : coffee(std::move(c)) {}

  double cost() override { return coffee->cost(); }

  std::string description() override { return coffee->description(); }
};

class MilkDecorator : public CoffeeDecorator {
public:
  explicit MilkDecorator(std::unique_ptr<Coffee> c)
      : CoffeeDecorator(std::move(c)){};

  double cost() override { return CoffeeDecorator::cost() + 2.0; }

  std::string description() override {
    return CoffeeDecorator::description() + " +Milk";
  }
};

// 具体装饰器：加糖
class SugarDecorator : public CoffeeDecorator {
public:
  explicit SugarDecorator(std::unique_ptr<Coffee> c)
      : CoffeeDecorator(std::move(c)) {}

  double cost() override { return CoffeeDecorator::cost() + 1.0; }

  std::string description() override {
    return CoffeeDecorator::description() + " + Sugar";
  }
};

void testDecorator() {
  // 创建一个基础咖啡
  auto coffee = std::make_unique<SimpleCoffee>();
  std::cout << coffee->description() << " cost $" << coffee->cost()
            << std::endl;

  // 加牛奶
  auto coffeeWithMilk =
      std::make_unique<MilkDecorator>(std::make_unique<SimpleCoffee>());
  std::cout << coffeeWithMilk->description() << " costs $"
            << coffeeWithMilk->cost() << std::endl;
}

// 5. 观察者模式

class Observer {
public:
  virtual void update(const std::string &message) = 0;
  virtual ~Observer() = default;
};

// 主题 (被观察者)

class Subject {
private:
  std::vector<std::shared_ptr<Observer>> observers;
  std::string state;

public:
  void attach(std::shared_ptr<Observer> observer) {
    observers.push_back(observer);
  }

  void detach(std::shared_ptr<Observer> observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer),
                    observers.end());
  }

  void setState(const std::string &newState) {
    state = newState;
    notify();
  }

private:
  void notify() {
    for (const auto &observer : observers) {
      observer->update(state);
    }
  }
};

// 具体观察者

class ConcreteObserver : public Observer {
private:
  std::string name;

public:
  explicit ConcreteObserver(std::string observerName)
      : name(std::move(observerName)) {}

  void update(const std::string &message) override {
    std::cout << name << " received message: " << message << std::endl;
  }
};

void testObserver() {
  // 创建主题

  Subject subject;

  // 创建观察者

  // 创建观察者
  auto observer1 = std::make_shared<ConcreteObserver>("Observer 1");
  auto observer2 = std::make_shared<ConcreteObserver>("Observer 2");
  auto observer3 = std::make_shared<ConcreteObserver>("Observer 3");

  // 添加观察者
  subject.attach(observer1);
  subject.attach(observer2);
  subject.attach(observer3);

  // 改变状态，所有观察者都会收到通知
  std::cout << "First state change:" << std::endl;
  subject.setState("Hello, Observers!");

  // 移除一个观察者
  subject.detach(observer2);

  // 再次改变状态，只有剩余的观察者会收到通知
  std::cout << "\nSecond state change:" << std::endl;
  subject.setState("Hello again!");
}

// 6. 责任链模式

// 请求类
class Request {
private:
  std::string type;
  int amount;

public:
  Request(const std::string &type, int amount) : type(type), amount(amount){};

  std::string getType() const { return type; }
  int getAmount() const { return amount; }
};

// 处理者抽象类
class Handler {
protected:
  std::shared_ptr<Handler> nextHandler;

public:
  virtual ~Handler() = default;

  void setNext(std::shared_ptr<Handler> hanler) { nextHandler = hanler; }

  virtual void handleRequest(const Request &request) = 0;
};

// 具体处理者：主管
class TeamLeader : public Handler {
public:
  void handleRequest(const Request &request) override {
    if (request.getType() == "leave" && request.getAmount() <= 1) {
      std::cout << "TeamLeader approves " << request.getAmount()
                << " day leave request" << std::endl;
    } else if (nextHandler) {
      std::cout << "TeamLeader forwards request to next handler" << std::endl;
      nextHandler->handleRequest(request);
    } else {
      std::cout << "Request cannot be handled" << std::endl;
    }
  }
};

// 具体处理者：经理
class Manager : public Handler {
public:
  void handleRequest(const Request &request) override {
    if (request.getType() == "leave" && request.getAmount() <= 3) {
      std::cout << "Manager approves " << request.getAmount()
                << " days leave request" << std::endl;
    } else if (request.getType() == "expense" && request.getAmount() <= 1000) {
      std::cout << "Manager approves $" << request.getAmount()
                << " expense request" << std::endl;
    } else if (nextHandler) {
      std::cout << "Manager forwards request to next handler" << std::endl;
      nextHandler->handleRequest(request);
    } else {
      std::cout << "Request cannot be handled" << std::endl;
    }
  }
};

// 具体处理者：总监
class Director : public Handler {
public:
  void handleRequest(const Request &request) override {
    if (request.getType() == "leave" && request.getAmount() <= 5) {
      std::cout << "Director approves " << request.getAmount()
                << " days leave request" << std::endl;
    } else if (request.getType() == "expense" && request.getAmount() <= 5000) {
      std::cout << "Director approves $" << request.getAmount()
                << " expense request" << std::endl;
    } else if (nextHandler) {
      std::cout << "Director forwards request to next handler" << std::endl;
      nextHandler->handleRequest(request);
    } else {
      std::cout << "Request cannot be handled" << std::endl;
    }
  }
};

// 具体处理者：CEO
class CEO : public Handler {
public:
  void handleRequest(const Request &request) override {
    if (request.getType() == "leave" && request.getAmount() <= 10) {
      std::cout << "CEO approves " << request.getAmount()
                << " days leave request" << std::endl;
    } else if (request.getType() == "expense" && request.getAmount() <= 10000) {
      std::cout << "CEO approves $" << request.getAmount() << " expense request"
                << std::endl;
    } else {
      std::cout << "Request denied" << std::endl;
    }
  }
};

void testChain_of_responsibility() {
  // 创建处理者链
  auto teamLeader = std::make_shared<TeamLeader>();
  auto manager = std::make_shared<Manager>();
  auto director = std::make_shared<Director>();
  auto ceo = std::make_shared<CEO>();

  // 设置处理者链
  teamLeader->setNext(manager);
  manager->setNext(director);
  director->setNext(ceo);

      // 测试不同类型的请求
    std::cout << "\nTesting leave requests:" << std::endl;
    Request leave1("leave", 1);    // 应该由TeamLeader处理
    Request leave3("leave", 3);    // 应该由Manager处理
    Request leave5("leave", 5);    // 应该由Director处理
    Request leave7("leave", 7);    // 应该由CEO处理
    Request leave12("leave", 12);  // 应该被拒绝

    teamLeader->handleRequest(leave1);
    std::cout << std::endl;
    teamLeader->handleRequest(leave3);
    std::cout << std::endl;
    teamLeader->handleRequest(leave5);
    std::cout << std::endl;
    teamLeader->handleRequest(leave7);
    std::cout << std::endl;
    teamLeader->handleRequest(leave12);
}
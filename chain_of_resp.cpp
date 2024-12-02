#include <iostream>
#include <memory>

class Context { // 这是一个请求方
public:
  std::string name;
  int day;
};

// 这些是处理对象 可以理解为这是一条部门批准的申请链
class HandleByMainProgram;
class HandleByProjMgr;
class HandleByBoss;
class HandleByBeauty;

class IHandler { // 这是一个接口，所有的处理对象都要继承
public:
  IHandler() : next(nullptr) {}
  virtual ~IHandler();
  void setNextHandler(IHandler *handler) { // 设置下一个处理对象 传递责任链
    next = handler;
  }
  bool
  Handle(const Context &ctx) { // 处理方法
                               // 先看自己能不能处理，不能处理传递给下一个对象
                               // 否则就打断
    if (CanHandle(ctx)) {
      return HandleRequest(ctx);
    } else if (GetNextHandler()) {
      return GetNextHandler()->Handle(ctx);
    } else {
      // err
    }
    return false;
  }

protected:
  virtual bool HandleRequest(const Context &ctx) = 0;
  virtual bool CanHandle(const Context &ctx) = 0;
  IHandler *GetNextHandler() { return next; }

private:
  IHandler *next; // 下一个处理对象
};

class HandleByMainProgram : public IHandler {
protected:  
  virtual bool HandleRequest(const Context &ctx) {
    std::cout << "HandleByMainProgram" << std::endl;
    return true;
  }
  virtual bool CanHandle(const Context &ctx) {
    return ctx.day <= 1;
  }
};

class HandleByProjMgr:public IHandler{
protected:
  virtual bool HandleRequest(const Context &ctx){
    std::cout << "HandleByProjMgr" << std::endl;
    return true;
  }
  virtual bool CanHandle(const Context &ctx){
    return ctx.day <=20;
  }
};


class HandleByBoss:public IHandler{
protected:
  virtual bool HandleRequest(const Context &ctx){
    std::cout << "HandleByBoss" << std::endl;
    return true;
  }
  virtual bool CanHandle(const Context &ctx){
    return ctx.day <30;
  }
};

class HandleByBeauty:public IHandler{
protected:
  virtual bool HandleRequest(const Context &ctx){
    std::cout << "HandleByBeauty" << std::endl;
    return true;
  }
  virtual bool CanHandle(const Context &ctx){
    return ctx.day <=3;
  }
};


class LeaveProcess {
public:
  LeaveProcess() {
    IHandler *h0 = new HandleByBeauty();
    IHandler *h1 = new HandleByMainProgram();
    IHandler *h2 = new HandleByProjMgr();
    IHandler *h3 = new HandleByBoss();
    h0->setNextHandler(h1);
    h1->setNextHandler(h2);
    h2->setNextHandler(h3);
    handler = h0;
  }
  bool Handle(const Context &ctx) { return handler->Handle(ctx); }
private:
  IHandler *handler;
};




int main(){
  // 抽象工厂

  // nginx http处理

  // 设置下一指针

  Context ctx;
  auto leave = std::make_shared<LeaveProcess>();
  leave->Handle(ctx);
  return 0;
}
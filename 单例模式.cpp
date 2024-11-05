
#if 0 // 饿汉模式
class Singleton {
private:
    // 私有的静态实例
    static Singleton instance;

    // 私有的构造函数
    Singleton()=default;

public:
    // 删除拷贝构造函数和赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // 公共的静态方法，用于获取实例
    static Singleton* getInstance() {
      return &instance;
    }
};

Singleton Singleton::instance;

#endif


#if 0 // 懒汉模式

class Singleton {
private:
    // 私有的静态实例
    static Singleton*  instance;

    // 私有的构造函数
    Singleton()=default;

public:
    // 删除拷贝构造函数和赋值操作符
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // 公共的静态方法，用于获取实例
    static std::shared_ptr<T>GetInstance(){
        static std::once_flag s_flag;
        std::call_once(s_flag,[&](){
        _instance=std::make_shared<T>();
        });
        return _instance;
  }
};

// 初始化静态成员变量
Singleton* Singleton::instance = nullptr;

#endif

#if 1 // c++11方法

class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }

    // ...
};

#endif


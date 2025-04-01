#include <iostream>

using namespace std;

enum class shape_type{
    circle,
    triangle,
    rectangle,
};

class shape {
public:
    virtual ~shape(){};
};

class circle : public shape {
public:
    ~circle() override {
      puts("~circle()");
    }

    void draw() {
      std::cout << "Drawing a circle." << std::endl;
    }
};

class triangle : public shape {
public:
    ~triangle() override {
      puts("~triangle()");
    }

    void draw() {
      std::cout << "Drawing a triangle." << std::endl;
    }
};

class rectangle : public shape {
public:
    ~rectangle() override {
      puts(" ~rectangle()");
    }

    void draw() {
      std::cout << "Drawing a rectangle." << std::endl;
    }
};

shape* create_shape(shape_type type){
  switch (type) {
    case shape_type::circle:
      return new circle();
    case shape_type::triangle:
      return new triangle();
    case shape_type::rectangle:
      return new rectangle();
    default:
      return new shape();
  }
}

/*引用计数*/
class share_count{
private:
    long m_count;
public:
    share_count()noexcept:m_count(1){}
    void add_count() noexcept{
      m_count++;
    }
    long reduce_count() noexcept{
      return --m_count;
    }
    long get_count() const noexcept{
      return m_count;
    }
};

/*
 * 要解引用*
 * 还要能用->
 * 还要能用在布尔表达式
 */

 /*share_ptr实现*/
template<typename T>
class smart_ptr{
template <typename U>
    friend class smart_ptr;
private:
    T* m_ptr;
    share_count* shareCount;
public:
    explicit smart_ptr(T *mPtr= nullptr) : m_ptr(mPtr) {
      if (mPtr){
        shareCount=new share_count();
      }
    }

    smart_ptr& operator =(smart_ptr rhs){
      rhs.swap(*this);
      return *this;
    };

    template<typename U>
    smart_ptr(const smart_ptr<U>& other){
      m_ptr=other.m_ptr;
      if (m_ptr){
        other.shareCount->add_count();
        shareCount=other.shareCount;
      }
    }
    template<typename U>
    smart_ptr(const smart_ptr<U>& other,T*ptr){
      m_ptr=ptr;
      if (m_ptr){
        other.shareCount->add_count();
        shareCount=other.shareCount;
      }
    }

    template<typename U>
    smart_ptr(smart_ptr<U>&& other)  {
      m_ptr=other.m_ptr;
      if (m_ptr){
        shareCount=other.shareCount;
        other.m_ptr= nullptr;
      }
    }
     T* get() const{
      return m_ptr;
    }
    T& operator*(){
      return *m_ptr;
    }
    long use_count() const{
      if (m_ptr){
        return shareCount->get_count();
      }else{return 0;}
    }
    T* operator->(){
      return m_ptr;
    }
    void swap(smart_ptr& rhs){
      using std::swap;
      swap(m_ptr,rhs.m_ptr);
      swap(shareCount,rhs.shareCount);
    }
    operator bool() const{
      return m_ptr;
    }
     ~smart_ptr(){
       printf("~smart_ptr(): %p\n", this);
       if (m_ptr&&!shareCount->reduce_count()){
         delete m_ptr;
         delete shareCount;
       }

    };
};

template <typename T>
void swap(smart_ptr<T>& lhs,smart_ptr<T>& rhs) noexcept{
  lhs.swap(rhs);
}
template<typename T,typename U>
smart_ptr<T>static_pointer_cast(const smart_ptr<U>& other) noexcept{
  T* ptr=static_cast<T*>(other.get());
  return smart_ptr<T>(other,ptr);
}

template<typename T,typename U>
smart_ptr<T>dynamic_pointer_cast(
        const smart_ptr<U>& other
)noexcept{
  T* ptr= dynamic_cast<T*>(other.get());
  return smart_ptr<T>(other,ptr);
}

template<typename T,typename U>
smart_ptr<T>reinterpret_pointer_cast(
        const smart_ptr<U>& other
) noexcept{
  T* ptr= reinterpret_cast<T*>(other.get());
  return smart_ptr<T>(other,ptr);
}

template<typename T,typename U>
smart_ptr<T>const_pointer_cast(
        const smart_ptr<U>& other
) noexcept{
  T* ptr= const_cast<T*>(other.get());
  return smart_ptr<T>(other,ptr);
}





/* uniqueptr实现 */
template <typename T>
class UniquePtr {
private:
    T* m_ptr; // 原始指针

public:
    // 构造函数
    explicit UniquePtr(T* ptr = nullptr) noexcept : m_ptr(ptr) {}
    
    // 禁用拷贝构造函数和拷贝赋值运算符
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;
    
    // 移动构造函数
    UniquePtr(UniquePtr&& other) noexcept : m_ptr(other.m_ptr) {
        other.m_ptr = nullptr; // 防止原对象析构时释放资源
    }
    
    // 移动赋值运算符
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            reset(); // 释放当前资源
            m_ptr = other.m_ptr;
            other.m_ptr = nullptr;
        }
        return *this;
    }
    
    // 析构函数
    ~UniquePtr() {
        reset();
    }
    
    // 解引用操作符
    T& operator*() const {
        return *m_ptr;
    }
    
    // 箭头操作符
    T* operator->() const {
        return m_ptr;
    }
    
    // 获取原始指针
    T* get() const noexcept {
        return m_ptr;
    }
    
    // 释放所有权并返回原始指针
    T* release() noexcept {
        T* temp = m_ptr;
        m_ptr = nullptr;
        return temp;
    }
    
    // 重置指针
    void reset(T* ptr = nullptr) noexcept {
        T* old_ptr = m_ptr;
        m_ptr = ptr;
        if (old_ptr) {
            delete old_ptr;
        }
    }
    
    // 交换两个unique_ptr
    void swap(UniquePtr& other) noexcept {
        std::swap(m_ptr, other.m_ptr);
    }
    
    // 布尔转换操作符
    explicit operator bool() const noexcept {
        return m_ptr != nullptr;
    }
};



int main()
{
  smart_ptr<circle>ptr1(new circle());
  cout<<"use count of ptr1 is "<<ptr1.use_count()<<endl;
  smart_ptr<shape>ptr2;
  cout<<"use count of ptr2 was "<<ptr2.use_count()<<endl;
  ptr2=ptr1;
  printf("use count of ptr2 is now %ld\n",ptr2.use_count());
  if (ptr1){
    puts("ptr1 is not empty");
  }
  smart_ptr<circle> ptr3= dynamic_pointer_cast<circle>(ptr2);
  printf("use count of ptr3 is %ld\n",ptr3.use_count());

}







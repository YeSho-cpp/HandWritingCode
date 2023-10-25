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







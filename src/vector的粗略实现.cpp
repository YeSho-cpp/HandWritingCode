#include <iostream>
#include <future>
#include <mutex>
#include <random>
#include <string>
#include <cstring>
//using namespace std;



// 定义容器的空间配置器，和C++标准库的allocator实现一样

template<typename T>
struct Allocator {
    T *allocate(size_t size)   // 负责开辟内存
    {
      return static_cast<T *>(malloc(size * sizeof(T)));
    }

    void deallocate(T *ptr)  // 负责释放内存
    {
      free(ptr);
    }

    void construct(T *p, const T &val)  // 负责对象构造
    {
      new(p) T(val);
    }

    void destory(T *p) // 负责对象析构
    {
      p->~T(); // ~T()代表了T类型的析构函数
    }
};


/*
容器底层内存开辟，内存释放，对象构造和析构，都通过allocator空间配置器来实现
 */

template<typename T, typename Alloc=Allocator<T>>
class vector {
private:
    T *_first; // 指向容器底层内存的起始位置
    T *_end; // 指向容器底层内存的结束位置
    T *_last; // 指向容器底层内存的可用结束位置
    Alloc _alloc; // 空间配置器对象
    void expand() // 容器二倍扩容
    {
      size_t size = _end - _first;
      T *tmp = _alloc.allocate(2 * size);
      memcpy(tmp, _first, size * sizeof(T));
      _alloc.deallocate(_first);
      _first = tmp;
      _last = _first + size;
      _end = _first + 2 * size;
    }

public:
    vector(int size = 10) {
      // 需要把内存开辟和对象构造分开处理
      _first = _alloc.allocate(size);
      _end = _first + size;
      _last = _first;
    }

    ~vector() {
      // 需要把对象析构和内存释放分开处理
      for (T *p = _first; p != _last; p++) {
        _alloc.destory(p);
      }
      _alloc.deallocate(_first);
      _first = _end = _last = nullptr;
    }

    vector(const vector<T> &rhs){
      int size=rhs._end-rhs._first;
      _first=_alloc.allocate(size);
      _end=_first+size;
      _last=_first+size;
      for(int i=0;i<size;i++){
          _alloc.construct(_first+i,rhs._first[i]);
      }
    }


    vector(const vector<T> &&rhs){
      _first=rhs._first;
      _end=rhs._end;
      _last=rhs._last;
      rhs._first=rhs._end=rhs._last=nullptr;
    }

    vector(const std::initializer_list<T>& init) {
      int size = init.size();
      _first = _alloc.allocate(size);
      _end = _first + size;
      _last = _first + size;
      int i = 0;
      for (const T& val : init) {
        _alloc.construct(_first + i, val);
        i++;
      }
    }

    vector<T>& operator=(const vector<T> &rhs){
      if(this==&rhs){
          return *this;
      }
      int size=rhs._end-rhs._first;
      _first=_alloc.allocate(size);
      _end=_first+size;
      _last=_first+size;
      for(int i=0;i<size;i++){
          _alloc.construct(_first+i,rhs._first[i]);
      }
      return *this;
    }

    void push_back(const T &val) {
      if (_last == _end) {
        expand();
      }
      _alloc.construct(_last, val);
      _last++;
    }

    void pop_back() {
      if (empty()) {
        return;
      }
      _alloc.destory(--_last);
    }

    T back() const {
      return *(_last - 1);
    }

    bool empty() const {
      return _first == _last;
    }
    bool full() const {
      return _last == _end;
    }
    int size() const {
      return _last - _first;
    }
    int capicity() const {
      return _end - _first;
    }
    T& operator[] (int index) {
      if (index<0 || index>=size()) {
        throw "out of range";
      }
      return _first[index];
    }

    // 迭代器一般实现成容器的嵌套类型
    class iterator {
        private:
        T *_ptr; // 迭代器指向容器底层内存的某个位置
        public:
        iterator(T *ptr=nullptr) : _ptr(ptr) {}
        bool operator!=(const iterator &rhs) const {
          return _ptr != rhs._ptr;
        }
        void operator++() {
          _ptr++;
        }
        T& operator*() const {
          return *_ptr;
        }
    };
    iterator begin() {
      return iterator(_first);
    }
    iterator end() {
      return iterator(_last);
    }
};


int main() {
  vector<int> v{1,23,3};

  return 0;
}

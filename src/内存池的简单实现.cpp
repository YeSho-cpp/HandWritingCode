#include <iostream>
#include <future>
#include <mutex>
#include <random>
#include <string>
#include <cstring>


// 创建一个模板队列，队列数据、next、有指向队尾和队头节点的
template<typename T>
class Queue{
private:
    struct QueueItem{  // 产生一个QueueItem的对象生成池
        QueueItem(T data=T()):_data(data),_next(nullptr) {}
        T _data;
        QueueItem *_next;
        static QueueItem *_itemPool; // todo _itemPool代表空闲位置的指针
        static const size_t POOL_ITEM_SIZE=100000;
        void * operator new (size_t size){

          if (_itemPool== nullptr){ // 没有内存池就要自己开辟
            _itemPool= (QueueItem *)new char[POOL_ITEM_SIZE*sizeof(QueueItem)];
            QueueItem* cur=_itemPool;
            for (int i = 0; i < POOL_ITEM_SIZE-1; ++i) {
              cur->_next=cur+1;
              cur=cur->_next;
            }
            cur->_next= nullptr;
          }
          // 有内存池的情况下,从_itemPool开始size个空闲使用
          QueueItem* cur=_itemPool;
          _itemPool=_itemPool->_next;
          return cur;
        }
        void operator delete (void *ptr){
          if (ptr== nullptr) return;

          QueueItem* p=(QueueItem *)ptr;
          p->_next=_itemPool;
          _itemPool=p;

        }
    };
    QueueItem *_front;
    QueueItem *_rear;
public:
    Queue(){
      _front=_rear= new QueueItem();
    }
    ~Queue(){
      QueueItem *cur=_front;
      while (cur!=_rear){
        _front=_front->_next;
        delete cur;
        cur=_front;
      }
    }
    void addQueue(const T& data){
      _rear->_next=new QueueItem(data);
      _rear=_rear->_next;
      _rear->_next= nullptr;
    }

    bool empty() {
      return _front==_rear;
    }

    void popQueue(){
      if (empty()) return;
      QueueItem *tmp=_front;
      _front=_front->_next;
      delete tmp;
    }
    T front() const{
      return _front->_data;
    }
};
template<typename T>
typename Queue<T>::QueueItem* Queue<T>::QueueItem::_itemPool= nullptr;
int main() {

  Queue<int>q;
  for (int i = 0; i < 10000; ++i) {
    q.addQueue(i);
    q.popQueue();
  }
  std::cout<<q.empty()<<std::endl;
  return 0;
}

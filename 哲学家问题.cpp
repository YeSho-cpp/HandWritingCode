#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>
using namespace std;


class DiningPhilosophers {
private:
  mutex forks[5];
  vector<vector<int>> res; // 记录结果
  mutex resMtx;   // 保护结果的互斥锁

public:
  void eat(int id){
    int left=id;
    int right=(id+1)%5;

    // 偶数先左后右，奇数先后后左
    if(id%2==0){
      forks[left].lock();
      forks[right].lock();
    }else{
      forks[right].lock();
      forks[left].lock();
    }

    // 记录结果
    lock_guard<mutex> lock(resMtx);
    res.push_back({id,0,3});

    // 放下叉子
    forks[left].unlock();
    forks[right].unlock();
  }

  vector<vector<int>> getResult() {
    return res;
  }
};


int main(){
  int n;
  cin>>n;

  DiningPhilosophers dp;
  vector<thread> threads;

  for(int i=0;i<5;i++){
    for(int j=0;j<n;j++){
      threads.emplace_back([&dp,i](){
        dp.eat(i);
      });
    }
  }

  // 等待所有线程结束

  for(auto& t:threads){
    t.join();
  }

  // 输出结果

  auto res=dp.getResult();

  for(auto&v:res){
    cout<<v[0]<<" "<<v[1]<<" "<<v[2]<<endl;
  }
  return 0;

}
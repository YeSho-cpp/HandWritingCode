#include <iostream>
#include <thread>
#include <mutex>

std::mutex g_mtx;

std::coniton_variable g_cv;
std::string flag = "0";

void PrintA(){
  while(true){
    std::unique_lock<std::mutex> lck(g_mtx);
    g_cv.wait(lck,[]{return flag=="0";});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "A" << std::endl;
    flag = "1";
    g_cv.notify_all();
  }
}

void PrintB(){
  while(true){
    std::unique_lock<std::mutex> lck(g_mtx);
    g_cv.wait(lck,[]{return flag=="1";});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "A" << std::endl;
    flag = "2";
    g_cv.notify_all();
  }
}

void PrintC(){
  while(true){
    std::unique_lock<std::mutex> lck(g_mtx);
    g_cv.wait(lck,[]{return flag=="2";});
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "A" << std::endl;
    flag = "3";
    g_cv.notify_all();
  }
}
#include <vector>
#include <iostream>
#include <forward_list>
#include <functional>
#include <list>
#include <map>
using namespace std;

// map+lambda实现一个计算器

map<char,function<double(double,double)>>ops={
        {'+',[](double a,double b){return a+b;}},
        {'-',[](double a,double b){return a-b;}},
        {'*',[](double a,double b){return a*b;}},
        {'/',[](double a,double b){return a/b;}},
};

int main()
{

  int a=34;
  int b=45;
  cout<<"请输入您要进行的运算"<<endl;
  char c;
  cin>>c;
  if (ops.find(c)!=ops.end()){
   cout<<"结果为："<<ops[c](a,b)<<endl;
  }
  return EXIT_SUCCESS;
}

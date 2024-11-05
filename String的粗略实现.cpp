#include <iostream>
#include <cstring>

using namespace std;

class String{

private:
  char *_str;
  friend ostream& operator<<(ostream &out,const String &s);
  friend String operator +(const String &s1,const String &s2);
public:
    //构造函数
    String(const char *ptr=nullptr){
      if (ptr== nullptr){
        _str=new char[1];
        *_str='\0';
      }
      else{
        _str=(char *)malloc(strlen(ptr)+1);
        strcpy(_str,ptr);
      }
    }
    ~String(){
      delete []_str;
      _str= nullptr;
    }
    String(const String &other){
      if (other._str== nullptr){
        _str=new char[1];
        *_str='\0';
      }
      else{
        _str=(char *) malloc(strlen(other._str)+1);
        strcpy(_str,other._str);
      }
    }
    String& operator =(const String& src){
      if (&src==this) return *this;
      delete []_str;
      _str=new char[strlen(src._str)+1];
      strcpy(_str,src._str);
      return *this;
    }
    String& operator =(const char *ptr){
      delete []_str;
      _str=new char[strlen(ptr)+1];
      strcpy(_str,ptr);
      return *this;
    }
    bool operator >(const String& src) const{
      return strcmp(this->_str,src._str)>0;
    }
    bool operator <(const String& src) const{
      return strcmp(this->_str,src._str)<0;
    }
    bool operator ==(const String& src) const
    {
      return strcmp(this->_str,src._str)==0;
    }
    int length() const{
      return strlen(this->_str);
    }
    char& operator [](int index){
      return _str[index];
    }
    const char& operator [](int index) const{
      return _str[index];
    }
    const char * c_str() const{
      return _str;
    }
    class iterator{
    public:
        iterator(char *p= nullptr)  :_p(p){}
        bool operator!=(const iterator &it){
          return _p !=it._p;
        }
        void operator++(){
          ++_p;
        }
        char& operator*() {return *_p;}
    private:
        char *_p;
    };
    iterator begin(){return iterator(_str);};
    iterator end(){return iterator(_str+length());}
};

String operator +(const String &s1,const String &s2){
  char *tmp=new char[s1.length()+s2.length()+1];
  strcpy(tmp,s1._str);
  strcat(tmp,s2._str);

  return {tmp};

}
ostream& operator<<(ostream &out,const String &s){

  out<<s._str;
  return out;
}
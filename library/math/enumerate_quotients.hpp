// https://github.com/soryuusi0219/compro/blob/main/library/math/enumerate_quotients.hpp
#pragma once
#include <algorithm>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
struct EnumerateQuotients{
  private:
    T N;
    vector<T> v;
  public:
    EnumerateQuotients(T n) : N(n){
      for(T i = N; i; i = N/(N/i+1)) v.push_back(i);
      reverse(v.begin(),v.end());
    }
    inline int size() const {return v.size();}
    inline T operator[](int i) const {return v[i];}
    inline T id(int n) {return (n*n < N ? n-1 : size()-N/n);}
};

}  // namespace soryuusi

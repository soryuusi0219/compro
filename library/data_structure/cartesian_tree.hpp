// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/cartesian_tree.hpp
#pragma once
#include <tuple>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T>
tuple<vector<int>,vector<int>,vector<int>,int> cartesian_tree(const vector<T>& v, bool b = false){
  int n = v.size();
  vector<int> p(n,-1);
  vector<int> l(n,-1);
  vector<int> r(n,-1);
  int top = 0;
  for(int i = 1; i < n; i++){
    p[i] = i-1;
    r[i-1] = i;
    while(p[i] >= 0 && ((v[i] > v[p[i]]) == b)){
      int j = p[i];
      p[i] = p[p[i]];
      if(p[i] >= 0) r[p[i]] = i;
      r[j] = l[i];
      if(r[j] >= 0) p[r[j]] = j;
      p[j] = i;
      l[i] = j;
    }
    if(p[i] == -1) top = i;
  }
  return {p,l,r,top};
}

}  // namespace soryuusi

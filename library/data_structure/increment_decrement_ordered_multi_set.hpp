// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/increment_decrement_ordered_multi_set.hpp
#pragma once
#include <numeric>
#include <utility>
#include <vector>
#include <cassert>

namespace soryuusi {

using namespace std;

struct IncrementDecrementOrderedMultiSet{
  private:
    int n, m;
    vector<int> v, p, q, bd;
  public:
    IncrementDecrementOrderedMultiSet(int n_, int m_) : n(n_), m(m_), v(n,0), p(n), q(n), bd(m,n) {
      iota(p.begin(),p.end(),0);
      iota(q.begin(),q.end(),0);
    }
    IncrementDecrementOrderedMultiSet(int n_, int m_, int x) : n(n_), m(m_), v(n,x), p(n), q(n), bd(m) {
      fill(bd.begin() + x, bd.end(), n);
      iota(p.begin(),p.end(),0);
      iota(q.begin(),q.end(),0);
    }
    void increment(int i){
      assert(v[i] < m);
      int k = v[i]++; --bd[k];
      swap(p[q[i]], p[bd[k]]);
      swap(q[i], q[p[q[i]]]);
    }
    void decrement(int i){
      assert(v[i] > 0);
      int k = --v[i];
      swap(p[q[i]], p[bd[k]]);
      swap(q[i], q[p[q[i]]]);
      ++bd[k];
    }
    inline int kth(int x) const {return p[x];}
    inline int count_lt(int x) const {return (x > m ? n : (x <= 0 ? 0 : bd[x-1]));}
    inline int count_interval(int x, int y) const {return count_lt(y) - count_lt(x);}
};


}  // namespace soryuusi

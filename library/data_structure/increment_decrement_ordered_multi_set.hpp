// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/increment_decrement_ordered_multi_set.hpp
#pragma once
#include <algorithm>
#include <cassert>
#include <vector>

namespace soryuusi {

using namespace std;

struct IncrementDecrementOrderedMultiSet{
  private:
    int n, m;
    struct Element {int value, rank;};
    vector<Element> elements;
    vector<int> p, bd;
    void move_to(int i, int rank){
      int old = elements[i].rank, j = p[rank];
      p[old] = j;
      p[rank] = i;
      elements[j].rank = old;
      elements[i].rank = rank;
    }
  public:
    IncrementDecrementOrderedMultiSet(int n_, int m_) : IncrementDecrementOrderedMultiSet(n_,m_,0){}
    IncrementDecrementOrderedMultiSet(int n_, int m_, int x) : n(n_), m(m_), elements(n), p(n), bd(m,n){
      assert(0 <= x && x <= m);
      fill_n(bd.begin(),x,0);
      for(int i = 0; i < n; ++i){elements[i] = {x,i}; p[i] = i;}
    }
    void increment(int i){
      assert(elements[i].value < m);
      int k = elements[i].value++;
      move_to(i,--bd[k]);
    }
    void decrement(int i){
      assert(elements[i].value > 0);
      int k = --elements[i].value;
      move_to(i,bd[k]++);
    }
    inline int kth(int x) const {return p[x];}
    inline int count_lt(int x) const {return (x > m ? n : (x <= 0 ? 0 : bd[x-1]));}
    inline int count_interval(int x, int y) const {return count_lt(y) - count_lt(x);}
};


}  // namespace soryuusi

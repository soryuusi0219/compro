// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/point_set_range_count.hpp
#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, auto hash>
struct PointSetRangeCount{
  static_assert(is_invocable_r_v<uint64_t, decltype(hash), T>, "hash must be callable with (T) and return a value convertible to uint64_t");

  private:
    struct Compressor{
      using u32 = uint32_t;
      using u64 = uint64_t;
      private:
        vector<T> key;
        vector<int> val;
        u32 mask;
        u32 index(T x) const {
          static const u32 R = []() -> u64 {
            u64 x = chrono::steady_clock::now().time_since_epoch().count();
            for(int i = 0; i < 8; i++) {
                x ^= x << 13;
                x ^= x >> 7;
                x ^= x << 17;
            }
            return x | 1;
          }() & mask;
          u64 i = hash(x) & mask;
          for(; ~val[i] && key[i] != x; i = (i+R) & mask);
          return i;
        }
        int mx = 0;
      public:
        Compressor(int n){
          int s = 1; while(s < n * 2) s <<= 1;
          key.resize(s);
          val.assign(s,-1);
          mask = s-1;
        }
        int add(const T &k){
          u32 id = index(k);
          if(!~val[id]) key[id] = k, val[id] = mx++;
          return val[id];

        }
        inline int get(const T &k) const {
          u32 id = index(k);
          return ~val[id] ? val[id] : -1;
        }
    } compressor;
    struct node{
      int pos, sum;
      node* lp;
      node* rp;
      node(int pos_, bool val_) : pos(pos_), sum(val_), lp(nullptr), rp(nullptr) {}
      inline bool val() const {return sum - (lp ? lp->sum : 0) - (rp ? rp->sum : 0);}
    };
    void _add(node*& p, int i){
      if(!p) p = new node(i,1);
      else __add(p, i);
    }
    void __add(node* p, int i){
      int l = 0, r = n;
      while(p->pos != i){
        int m = (l+r) >> 1;
        if(i < m){
          if(i > p->pos) {
            swap(p->pos,i);
            if(!(p->val())) break;
          }
          ++(p->sum), r = m;
          if(!p->lp) p->lp = new node(i,0);
          p = p->lp;
        }else{
          if(i < p->pos) {
            swap(p->pos,i);
            if(!(p->val())) break;
          }
          ++(p->sum), l = m;
          if(!p->rp) p->rp = new node(i,0);
          p = p->rp;
        }
      }
      ++(p->sum);
    }

    void _del(node* p, int i){
      int l = 0, r = n;
      while(p->pos != i){
        --p->sum;
        int m = (l+r) >> 1;
        if(i < m) r = m, p = p->lp;
        else l = m, p = p->rp;
      }
      --(p->sum);
    }

    int _count(node* p, int i) const {
      int res = 0;
      while(p && p->pos != i){
        if(i < p->pos) p = p->lp;
        else{
          res += p->sum, p = p->rp;
          if(p) res -= p->sum;
        }
      }
      if(p && p->lp) res += p->lp->sum;
      return res;
    }

    inline int _count(node* p, int l, int r) const { return _count(p,r) - _count(p,l);}
    int n;
    vector<int> v;
    vector<node*> inst;
  public:
    PointSetRangeCount(int n_, int m) : compressor(m), n(n_), v(n,-1), inst(m, nullptr) {}
    PointSetRangeCount(const vector<T>& a, int m) : compressor(m), n(a.size()), v(n), inst(m, nullptr) {
      for(int i = 0; i < n; ++i) _add(inst[v[i]=compressor.add(a[i])],i);
    }
    void set(int i, const T& k){
      if(~v[i]) _del(inst[v[i]],i);
      _add(inst[v[i]=compressor.add(k)],i);
    }
    int count(int l, int r, const T& k) const {
      int id = compressor.get(k);
      if(!~id) return 0;
      return _count(inst[id],l,r);
    }
};
template<class T>
uint64_t hash_int(T x){
  static const uint64_t R = []() -> uint64_t {
    uint64_t x = chrono::steady_clock::now().time_since_epoch().count();
    for(int i = 0; i < 8; i++) {
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
    }
    return x;
  }();
  uint64_t res = static_cast<uint64_t>(x);
  res += R;
  res = (res ^ (res >> 30)) * 0xbf58476d1ce4e5b9;
  res = (res ^ (res >> 27)) * 0x94d049bb133111eb;
  return (res ^ (res >> 31));
}

}  // namespace soryuusi

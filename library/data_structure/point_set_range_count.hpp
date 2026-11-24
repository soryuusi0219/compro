// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/point_set_range_count.hpp
#pragma once
#include <cstddef>
#include <utility>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "hash_table.hpp"
#include "internal/hash_table_storage.hpp"
#include "internal/node_pool.hpp"

namespace soryuusi {

using namespace std;

template<class T, auto hash>
struct PointSetRangeCount{
  static_assert(is_invocable_r_v<uint64_t, decltype(hash), T>, "hash must be callable with (T) and return a value convertible to uint64_t");

  private:
    struct Compressor{
      // lookup() does not change the table. Keep the public count() const.
      mutable internal::HashTableStorage<T,int,hash> values;
      int size = 0, limit;

      explicit Compressor(int m) : values(m), limit(m) {}
      int add(const T& key){
        auto i = values.insert(key,[&]{
          if(size == limit) throw length_error("PointSetRangeCount value capacity exceeded");
          return size++;
        });
        return values.value(i);
      }
      int get(const T& key) const {
        auto i = values.lookup(key);
        return i == values.end() ? -1 : values.value(i);
      }
    } compressor;
    struct node{
      int pos, sum;
      node* lp;
      node* rp;
      node(int pos_, bool val_) : pos(pos_), sum(val_), lp(nullptr), rp(nullptr) {}
      inline bool val() const {return sum - (lp ? lp->sum : 0) - (rp ? rp->sum : 0);}
    };
    internal::NodePool<node> pool;
    void _add(node*& p, int i){
      if(!p) p = pool.create(i,1);
      else __add(p, i);
    }
    void __add(node* p, int i){
      int l = 0, r = n;
      while(p->pos != i){
        int m = l+(r-l)/2;
        if(i < m){
          if(i > p->pos) {
            swap(p->pos,i);
            if(!(p->val())) break;
          }
          ++(p->sum), r = m;
          if(!p->lp) p->lp = pool.create(i,0);
          p = p->lp;
        }else{
          if(i < p->pos) {
            swap(p->pos,i);
            if(!(p->val())) break;
          }
          ++(p->sum), l = m;
          if(!p->rp) p->rp = pool.create(i,0);
          p = p->rp;
        }
      }
      ++(p->sum);
    }

    void _del(node* p, int i){
      int l = 0, r = n;
      while(p->pos != i){
        --p->sum;
        int m = l+(r-l)/2;
        if(i < m) r = m, p = p->lp;
        else l = m, p = p->rp;
      }
      --(p->sum);
    }

    int _count(const node* p, int i) const {
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

    int _count(const node* p, int l, int r) const {
      while(p && p->sum){
        if(r <= p->pos) p = p->lp;
        else if(p->pos < l) p = p->rp;
        else {
          int taken = p->sum - (p->rp ? p->rp->sum : 0);
          return taken-_count(p->lp,l)+_count(p->rp,r);
        }
      }
      return 0;
    }
    int n;
    vector<int> v;
    vector<node*> inst;
  public:
    PointSetRangeCount(const PointSetRangeCount&) = delete;
    PointSetRangeCount& operator=(const PointSetRangeCount&) = delete;
    PointSetRangeCount(PointSetRangeCount&& other) : PointSetRangeCount(0,0) {*this = std::move(other);}
    PointSetRangeCount& operator=(PointSetRangeCount&& other) {
      if(this == &other) return *this;
      compressor = std::move(other.compressor);
      n = std::move(other.n);
      v = std::move(other.v);
      inst = std::move(other.inst);
      pool = std::move(other.pool);
      other.n = 0;
      other.v.clear();
      other.inst.clear();
      return *this;
    }
    [[nodiscard]] PointSetRangeCount clone() const {
      PointSetRangeCount result(n,inst.size());
      result.compressor = compressor;
      result.v = v;
      auto dfs = [&](auto&& self, node* p) -> node* {
        if(!p) return nullptr;
        auto q = result.pool.create(*p);
        q->lp = self(self,p->lp); q->rp = self(self,p->rp);
        return q;
      };
      for(int i = 0; i < (int)inst.size(); ++i) result.inst[i] = dfs(dfs,inst[i]);
      return result;
    }
    std::size_t allocated_nodes() const {return pool.size();}
    PointSetRangeCount(int n_, int m) : compressor(m), n(n_), v(n,-1), inst(m, nullptr) {}
    PointSetRangeCount(const vector<T>& a, int m) : compressor(m), n(a.size()), v(n), inst(m, nullptr) {
      for(int i = 0; i < n; ++i) _add(inst[v[i]=compressor.add(a[i])],i);
    }
    void set(int i, const T& k){
      assert(0 <= i && i < n);
      int value = compressor.add(k);
      if(value == v[i]) return;
      _add(inst[value],i);
      if(~v[i]) _del(inst[v[i]],i);
      v[i] = value;
    }
    int count(int l, int r, const T& k) const {
      assert(0 <= l && l <= r && r <= n);
      if(l == r) return 0;
      int id = compressor.get(k);
      if(!~id) return 0;
      return _count(inst[id],l,r);
    }
};
}  // namespace soryuusi

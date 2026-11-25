// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/binary_trie.hpp
#pragma once
#include <cassert>
#include <cstddef>
#include <utility>

#include "internal/node_pool.hpp"

namespace soryuusi {

template<class T, int B = 60>
struct Binary_Trie{
  private:
    struct node {
      node* ch[2] = {};
      int sz = 0;

      void calc(){
        sz = (ch[0] ? ch[0]->sz : 0)+(ch[1] ? ch[1]->sz : 0);
      }
    };
    node* root = nullptr;
    T mask = 0;
    internal::NodePool<node> pool;

    bool _insert(node*& p, T x, int h){
      if(!p) p = pool.create();
      if(h == 0){
        if(p->sz) return false;
        p->sz = 1;
        return true;
      }
      if(!_insert(p->ch[(x >> (h-1)) & 1],x,h-1)) return false;
      ++p->sz;
      return true;
    }
    bool _erase(node* p, T x, int h){
      if(!p || p->sz == 0) return false;
      if(h == 0){
        p->sz = 0;
        return true;
      }
      if(!_erase(p->ch[(x >> (h-1)) & 1],x,h-1)) return false;
      --p->sz;
      return true;
    }
    void _merge(node*& p, const node* q, int h, T difference){
      if(!q || q->sz == 0) return;
      if(!p) p = pool.create();
      if(h == 0){
        p->sz = 1;
        return;
      }
      int flip = (difference >> (h-1)) & 1;
      _merge(p->ch[0],q->ch[flip],h-1,difference);
      _merge(p->ch[1],q->ch[flip^1],h-1,difference);
      p->calc();
    }

  public:
    Binary_Trie() = default;
    Binary_Trie(const Binary_Trie&) = delete;
    Binary_Trie& operator=(const Binary_Trie&) = delete;
    Binary_Trie(Binary_Trie&& other) noexcept : root(std::exchange(other.root,nullptr)), mask(std::exchange(other.mask,T(0))), pool(std::move(other.pool)){}
    Binary_Trie& operator=(Binary_Trie&& other) noexcept {
      if(this != &other){
        pool = std::move(other.pool);
        root = std::exchange(other.root,nullptr);
        mask = std::exchange(other.mask,T(0));
      }
      return *this;
    }
    [[nodiscard]] Binary_Trie clone() const {
      Binary_Trie result;
      auto dfs = [&](auto&& self,const node* p) -> node* {
        if(!p) return nullptr;
        auto q = result.pool.create(*p);
        q->ch[0] = self(self,p->ch[0]);
        q->ch[1] = self(self,p->ch[1]);
        return q;
      };
      result.root = dfs(dfs,root);
      result.mask = mask;
      return result;
    }
    void merge(Binary_Trie&& other){
      assert(this != &other);
      if(!root){
        *this = std::move(other);
        return;
      }
      _merge(root,other.root,B,mask^other.mask);
      other.root = nullptr;
    }
    std::size_t allocated_nodes() const {return pool.size();}
    int size() const {return root ? root->sz : 0;}
    void insert(T x){_insert(root,x^mask,B);}
    void erase(T x){_erase(root,x^mask,B);}
    void operate_xor(T x){if(root) mask ^= x;}
    T kth(int k){
      if(k < 0 || k >= size()) return T(-1);
      const node* p = root;
      T result = 0;
      for(int h = B-1; h >= 0; --h){
        int zero = (mask >> h) & 1;
        const node* left = p->ch[zero];
        int count = left ? left->sz : 0;
        if(k < count) p = left;
        else {
          k -= count;
          result |= T(1) << h;
          p = p->ch[zero^1];
        }
      }
      return result;
    }
    T min_element(){return kth(0);}
    T max_element(){return kth(size()-1);}
};

}  // namespace soryuusi

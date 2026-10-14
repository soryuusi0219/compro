// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/binary_trie.hpp
#pragma once
#include <memory>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, int B = 60>
struct Binary_Trie{
  private:
    struct node{
      node* lp;
      node* rp;
      int sz;
      T lz;
      node() : lp(nullptr), rp(nullptr), sz(0), lz(0){};
      void calc(){
        sz = (lp ? lp->sz : 0) + (rp ? rp->sz : 0);
      }
      void act(T f, int h){
        if(h){
          if((f >> (h-1)) & 1) swap(lp,rp);
          lz ^= f;
        }
      }
      void push(int h){
        if(lz){
          if(lp) lp->act(lz, h-1);
          if(rp) rp->act(lz, h-1);
          lz = 0;
        }
      }
    };
    node* root;
    vector<unique_ptr<node>> pool;
    node* make_node(){
      pool.push_back(make_unique<node>(node()));
      return pool.back().get();
    }
    void _insert(node*& p, T x, int h){
      if(!p) p = make_node();
      if(!h) {
        p->sz = 1;
        return;
      }
      p->push(h);
      if((x >> (--h)) & 1) _insert(p->rp, x, h);
      else _insert(p->lp, x, h);
      p->calc();
    }
    void _erase(node* p, T x, int h){
      if(!p) return;
      if(!h) {
        p->sz = 0;
        return;
      }
      p->push(h);
      if((x >> (--h)) & 1) _erase(p->rp, x, h);
      else _erase(p->lp, x, h);
      p->calc();
    }
    bool _contains(node* p, T x, int h){
      if(!p || !(p->sz)) return false;
      if(!h) return true;
      p->push(h);
      if((x >> (--h)) & 1) return _contains(p->rp, x, h);
      else return _contains(p->lp, x, h);
    }
    T _kth(node*& p, int k, int h){
      if(!h) return 0;
      p->push(h--);
      if(p->lp){
        if(k < p->lp->sz) return _kth(p->lp,k,h);
        k -= p->lp->sz;
      }
      return _kth(p->rp,k,h) | (((T)1) << h);
    }
    T _min_element(node*& p, int h){
      if(!h) return 0;
      p->push(h--);
      if(p->lp && p->lp->sz) return _min_element(p->lp,h);
      return _min_element(p->rp,h) | (((T)1) << h);
    }
    T _max_element(node*& p, int h){
      if(!h) return 0;
      p->push(h--);
      if(p->rp && p->rp->sz) return _min_element(p->rp,h) | (((T)1) << h);
      return _min_element(p->lp,h);
    }
  public:
    Binary_Trie(){root = make_node();}
    ~Binary_Trie(){pool.clear();}
    int size(){return root->sz;}
    void insert(T x){_insert(root, x, B);}
    void erase(T x){_erase(root, x, B);}
    void operate_xor(T x){return root->act(x,B);}
    T kth(int k){if(k < 0 || k >= size()) return -1; return _kth(root,k,B);}
    T min_element(){if(!size()) return -1; return _min_element(root,B);}
    T max_element(){if(!size()) return -1; return _max_element(root,B);}
};

}  // namespace soryuusi

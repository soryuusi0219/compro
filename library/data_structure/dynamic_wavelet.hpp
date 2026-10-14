// https://github.com/soryuusi0219/compro/blob/main/library/data_structure/dynamic_wavelet.hpp
#pragma once
#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <memory>
#include <random>
#include <stack>
#include <string>
#include <utility>
#include <vector>

namespace soryuusi {

using namespace std;

template<class T, int B = 31>
struct DynamicWavelet{
  private:
    struct DynamicBitVector {
      static constexpr int MAXBIT = 32;
      using u32 = uint32_t;
      using u64 = uint64_t;
      private:
        static unsigned long rng() {
          static random_device seed;
          static unsigned long x = seed(), y = seed(), z = seed(), w = seed();
          // static unsigned long x = 123456789, y = 362436069, z = 521288629, w = 88675123;
          unsigned long t = (x ^ (x << 11));
          x = y;
          y = z;
          z = w;
          return (w = (w ^ (w >> 19)) ^ (t ^ (t >> 8)));
        }
        struct node{
          node* pp = nullptr;
          node* lp = nullptr;
          node* rp = nullptr;
          u64 v;
          int pop, sumpop, z, sumz;
          u32 pri;
          node() : v(0), pop(0), sumpop(0), z(0), sumz(0), pri(rng()){}
          node(u64 v_, int w) : v(v_), pop(__builtin_popcountll(v)), sumpop(pop), z(w), sumz(w), pri(rng()){}
          void calc(){
            sumpop = pop, sumz = z;
            if(rp) sumpop += rp->sumpop, sumz += rp->sumz;
            if(lp) sumpop += lp->sumpop, sumz += lp->sumz;
          }
          void insert(int i, bool b){
            v += v & -(1ULL << i), z++, sumz++;
            if(b) v |= 1ULL << i, pop++, sumpop++;
          }
          void erase(int i){
            bool b = (v >> i) & 1;
            if(b) v &= ~(1ULL << i), pop--, sumpop--;
            v -= (v & -(1ULL << i)) >> 1, z--, sumz--;
          }
          bool get(int i){return (v >> i) & 1;}
          int rank(int i){return __builtin_popcountll(v & ((1Ull << i) - 1));}
          u64 split(){
            u64 vu = v >> MAXBIT;
            v &= (1ULL << MAXBIT) - 1;
            pop = __builtin_popcountll(v), z = MAXBIT;
            calc();
            return vu;
          }
          void rotr(){
            node* q = pp;
            node* r = q->pp;
            q->lp=rp;
            if(rp) rp->pp = q;
            rp = q;
            q->pp = this;
            pp = r;
            if(r){
              if(r->lp == q) r->lp = this;
              if(r->rp == q) r->rp = this;
            }
            q->calc();
            calc();
            if(r) r->calc();
          }
          void rotl(){
            node* q = pp;
            node* r = q->pp;
            q->rp=lp;
            if(lp) lp->pp = q;
            lp = q;
            q->pp = this;
            pp = r;
            if(r){
              if(r->lp == q) r->lp = this;
              if(r->rp == q) r->rp = this;
            }
            q->calc();
            calc();
            if(r) r->calc();
          }
        };
        using pnode = unique_ptr<node>;
        vector<pnode> nodes;
        node* most_left(node* c){
          if(!c) return nullptr;
          while(c -> lp) c = c -> lp;
          return c;
        }
        node* most_right(node* c){
          if(!c) return nullptr;
          while(c -> rp) c = c -> rp;
          return c;
        }
        node* kth_node(node* c, int& k){
          while(true){
            if(c->lp){
              if(c->lp->sumz > k){
                c = c->lp;
                continue;
              }
              k -= c->lp->sumz;
            }
            if(!c->rp || k < c->z) return c;
            k -= c->z;
            c = c->rp;
          }
        }
        node* make_node(){
          nodes.push_back(make_unique<node>(node()));
          return nodes.back().get();
        }
        node* make_node(u64 v){
          nodes.push_back(make_unique<node>(node(v, MAXBIT)));
          return nodes.back().get();
        }
        node* root = nullptr;
      public:
        DynamicBitVector() : root(nullptr){}
        DynamicBitVector(const vector<bool>& v){build(v);}
        void build(const vector<bool>& v){
          if(v.empty()) return;
          int n = v.size();
          int cnt = 0;
          u64 u = 0;

          stack<node*> st;
          for(int i = 0; i < n; ++i) {
            if(v[i]) u |= 1ULL << cnt;
            if(++cnt == MAXBIT || i == n-1){
              node* c = make_node(u);
              u32 pri = c->pri;
              while(!st.empty() && st.top()->pri < pri){
                node* d = st.top();
                d->pp = c;
                d->rp = c->lp;
                if(c->lp) c->lp->pp = d;
                c->lp = d;
                d->calc();
                st.pop();
              }
              st.push(c);
              cnt = 0, u = 0;
            }
          }
          while(st.size() > 1){
            node* c = st.top(); st.pop();
            node* d = st.top();
            c->calc();
            c->pp = d;
            d->rp = c;
          }
          root = st.top();
          root->calc();
        }

        int size(){return root ? root->sumz : 0;}
        void insert(int k, bool f){
          if(!root){
            root = make_node();
            root->insert(0,f);
            return;
          }
          node* c = kth_node(root,k);
          c->insert(k,f);
          if(c->z == MAXBIT*2){
            node* d = make_node(c->split());
            if(!c->rp) {
              c->rp = d;
              d->pp = c;
            }else{
              c = c->rp;
              while(c->lp) c = c->lp;
              c->lp = d;
              d->pp = c;
            }
            while(d->pp && d->pri > d->pp->pri){
              if(d->pp->lp == d) d->rotr();
              else d->rotl();
            }
            if(!d->pp) root = d;
            while((d = d->pp)) d->calc();
          }else while((c = c->pp)) c->calc();
        }
        void erase(int k){
          node* c = kth_node(root,k);
          c->erase(k);
          while((c = c->pp)) c->calc();
        }
        bool get(int k){
          node* c = kth_node(root,k);
          return c->get(k);
        }
        int rank(int k){
          node* c = root;
          int res = 0;
          while(true){
            if(c->lp){
              if(c->lp->sumz > k){
                c = c->lp;
                continue;
              }
              k -= c->lp->sumz;
              res += c->lp->sumpop;
            }
            if(!c->rp || k < c->z) return res + c->rank(k);
            k -= c->z;
            res += c->pop;
            c = c->rp;
          }
        }
      
        // void print(node* c = nullptr, string indent = ""){
        //   if(indent.size() > 20) return;
        //   if(c == nullptr) c = root;
        //   if(c->lp) print(c->lp,indent + "   ");

        //   cout << indent;
        //   cout << "[";
        //   cout << " p:"; if(c->pp) cout << c->pp; else cout << " ";
        //   cout << " r:"; if(c->rp) cout << c->rp; else cout << " ";
        //   cout << " l:"; if(c->lp) cout << c->lp; else cout << " ";
        //   cout << " v:" << c->v;
        //   cout << " z:" << c->z;
        //   cout << " sumz:" << c->sumz;
        //   cout << " pop:" << c->pop;
        //   cout << " sumpop:" << c->sumpop;
        //   cout << " pri:" << c->pri;
        //   cout << " adress:" << c;
        //   cout << "]" << endl;

        //   if(c->rp) print(c->rp,indent + "   ");
        // }
    };
    int n;
    int z[B];
    DynamicBitVector data[B];
  public:
    DynamicWavelet(vector<T> v) : n(v.size()){
      vector<T> vl, vr;
      vl.reserve(n);
      vr.reserve(n);
      vector<bool> b(n);
      for(int k = B-1; k >= 0; k--) {
        vl.clear();
        vr.clear();
        for(int i = 0; i < n; i++){
          if(b[i] = !((v[i] >> k) & 1)) vl.push_back(v[i]);
          else vr.push_back(v[i]);
        }
        data[k].build(b);
        z[k] = vl.size();
        v.swap(vl);
        v.insert(v.end(), vr.begin(), vr.end());
      }
    }

    int size(){return n;}

    T get(int i){
      T res = 0;
      for(int k = B-1; k >= 0; k--){
        res <<= 1;
        int i0 = data[k].rank(i);
        if(!data[k].get(i)) res++, i += z[k] - i0;
        else i = i0;
      }
      return res;
    }

    void insert(int i, T x){
      assert(0 <= i && i <= n);
      ++n;
      for(int k = B-1; k >= 0; --k){
        data[k].insert(i,!((x >> k) & 1));
        int i0 = data[k].rank(i);
        if((x >> k) & 1) i += z[k] - i0;
        else i = i0, ++z[k];
      }
    }
  
    void erase(int i){
      assert(0 <= i && i < n);
      --n;
      for(int k = B-1; k >= 0; --k){
        int i0 = data[k].rank(i);
        bool f = !data[k].get(i);
        data[k].erase(i);
        if(f) i += z[k] - i0;
        else i = i0, --z[k];
      }
    }

    void set(int i, T x){erase(i); insert(i,x);}

    int rank(int l, int r, T x){
      if(!n) return 0;
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank(l), r0 = data[k].rank(r);
        if((x >> k) & 1) l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return r-l;
    }

    T range_kth(int l, int r, int x){
      T res = 0;
      for(int k = B-1; k >= 0; k--){
        res <<= 1;
        int l0 = data[k].rank(l), r0 = data[k].rank(r);
        if(x >= r0 - l0) res++, x -= r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_lt(int l, int r, T x){
      if(!n) return 0;
      int res = 0;
      for(int k = B-1; k >= 0; k--){
        int l0 = data[k].rank(l), r0 = data[k].rank(r);
        if((x >> k) & 1) res += r0 - l0, l += z[k] - l0, r += z[k] - r0;
        else l = l0, r = r0;
      }
      return res;
    }

    int count_interval(int l, int r, T xl, T xu){return count_lt(l,r,xu) - count_lt(l,r,xl);}

    // min v[i] s.t. v[i] >= x
    T next(int l, int r, T x, T default_value = -1){
      int cnt = count_lt(l,r,x);
      return cnt == r-l ? default_value : range_kth(l,r,cnt);
    }
    
    // max v[i] s.t. v[i] < x
    T prev(int l, int r, T x, T default_value = -1){
      int cnt = count_lt(l,r,x);
      return cnt == 0 ? default_value : range_kth(l,r,cnt-1);
    }
};

}  // namespace soryuusi

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include "library/data_structure/dynamic_segtree.hpp"
#include "library/data_structure/persistent_dynamic_lazy_segtree.hpp"
using S = std::pair<long long,long long>;
long long add(long long a,long long b){return a+b;}
long long zero(){return 0;}
S op(S a,S b){return {a.first+b.first,a.second+b.second};}
S e(){return {0,0};}
S mp(long long f,S x){return {x.first+f*x.second,x.second};}
long long cp(long long f,long long g){return f+g;}
int main(int argc,char** argv){
  std::mt19937 rng(12345);
  auto start = std::chrono::steady_clock::now();
  long long sum = 0;
  if(argc > 1 && std::string(argv[1]) == "persistent"){
    soryuusi::Persistent_Dynamic_LazySegtree<S,op,e,long long,mp,cp,zero> seg(1<<20,[](int l,int r){return S{0,r-l};});
    auto root = seg.instance();
    for(int i = 0; i < 10000; ++i){int l = rng()%(1<<20), r = rng()%(1<<20); if(l>r)std::swap(l,r); root = seg.apply(root,l,r,1);}
    for(int i = 0; i < 100000; ++i){int l = rng()%(1<<20), r = rng()%(1<<20); if(l>r)std::swap(l,r); sum += seg.prod(root,l,r).first;}
    auto stop = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration<double>(stop-start).count() << ' ' << sum << '\n';
  }else{
    soryuusi::Dynamic_Segtree<long long,add,zero> seg(1<<30);
    for(int i = 0; i < 200000; ++i) seg.set(rng()%(1<<30),i);
    for(int i = 0; i < 200000; ++i){int l=rng()%(1<<30),r=rng()%(1<<30);if(l>r)std::swap(l,r);sum+=seg.prod(l,r);}
    auto stop = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration<double>(stop-start).count() << ' ' << sum << '\n';
  }
}

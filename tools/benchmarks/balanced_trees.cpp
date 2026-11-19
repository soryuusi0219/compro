#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "library/data_structure/dynamic_wavelet.hpp"
#include "library/data_structure/link_cut.hpp"
#include "library/data_structure/lazy_link_cut.hpp"
#include "library/data_structure/splay_tree.hpp"
#include "library/data_structure/treap.hpp"

long long add(long long a,long long b){return a+b;}
long long zero(){return 0;}
using S = std::pair<long long,int>;
S op(S a,S b){return {a.first+b.first,a.second+b.second};}
S e(){return {0,0};}
S mapping(long long f,S x){return {x.first+f*x.second,x.second};}

template<class Arena> long long sequence_ranges(int n,int q,std::mt19937& rng){
  Arena arena;
  auto seq = arena.new_sequence(std::vector<long long>(n,1));
  long long checksum = 0;
  for(int i=0;i<q;++i){
    int l=rng()%n,r=rng()%n;
    if(l>r) std::swap(l,r);
    if(i&1) checksum+=seq.prod(l,r+1);
    else seq.flip(l,r+1);
  }
  return checksum;
}

template<class Arena> long long lazy_sequence_ranges(int n,int q,std::mt19937& rng){
  Arena arena;
  auto seq = arena.new_sequence(std::vector<S>(n,S{1,1}));
  long long checksum = 0;
  for(int i=0;i<q;++i){
    int l=rng()%n,r=rng()%n;
    if(l>r) std::swap(l,r);
    if(i&1) checksum+=seq.prod(l,r+1).first;
    else seq.apply(l,r+1,1);
  }
  return checksum;
}

int main(int argc,char** argv){
  const std::string mode = argc > 1 ? argv[1] : "splay";
  constexpr int n = 50000, q = 100000;
  std::mt19937 rng(51245);
  long long checksum = 0;
  const auto start = std::chrono::steady_clock::now();
  if(mode == "splay"){
    checksum=sequence_ranges<soryuusi::SplayTree<long long,add,zero>>(n,q,rng);
  }else if(mode == "treap"){
    checksum=sequence_ranges<soryuusi::TreapSequence<long long,add,zero>>(n,q,rng);
  }else if(mode == "lazy_splay"){
    checksum=lazy_sequence_ranges<soryuusi::LazySplayTree<S,op,e,long long,mapping,add,zero>>(n,q,rng);
  }else if(mode == "lazy_treap"){
    checksum=lazy_sequence_ranges<soryuusi::LazyTreapSequence<S,op,e,long long,mapping,add,zero>>(n,q,rng);
  }else if(mode == "lct"){
    soryuusi::linkcuttree<long long,add,zero> tree(n);
    for(int i=0;i<n;++i)tree.set(i,1);
    for(int i=1;i<n;++i)tree.link(i-1,i);
    for(int i=0;i<q;++i){int u=rng()%n,v=rng()%n;if(i&1)checksum+=tree.prod(u,v);else tree.set(u,i%7);}
  }else if(mode == "lazy_lct"){
    soryuusi::lazy_linkcuttree<S,op,e,long long,mapping,add,zero> tree(n,S{1,1});
    for(int i=1;i<n;++i)tree.link(i-1,i);
    for(int i=0;i<q;++i){int u=rng()%n,v=rng()%n;if(i&1)checksum+=tree.prod(u,v).first;else tree.apply(u,v,1);}
  }else if(mode == "wavelet"){
    std::vector<int> a(n);for(auto& x:a)x=rng()%1024;
    soryuusi::DynamicWavelet<int,20> wavelet(a);
    for(int i=0;i<q;++i){int l=rng()%n,r=rng()%n,x=rng()%1024;if(l>r)std::swap(l,r);++r;if(i&1)checksum+=wavelet.rank(l,r,x);else wavelet.set(l,x);}
  }else return 1;
  const auto end = std::chrono::steady_clock::now();
  std::cout << std::chrono::duration<double>(end-start).count() << ' ' << checksum << '\n';
}

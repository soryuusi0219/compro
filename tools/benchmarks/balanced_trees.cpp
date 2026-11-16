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
#include "library/data_structure/splay_tree_lazy.hpp"

long long add(long long a,long long b){return a+b;}
long long zero(){return 0;}
using S = std::pair<long long,int>;
S op(S a,S b){return {a.first+b.first,a.second+b.second};}
S e(){return {0,0};}
S mapping(long long f,S x){return {x.first+f*x.second,x.second};}

int main(int argc,char** argv){
  const std::string mode = argc > 1 ? argv[1] : "splay";
  constexpr int n = 50000, q = 100000;
  std::mt19937 rng(51245);
  long long checksum = 0;
  const auto start = std::chrono::steady_clock::now();
  if(mode == "splay"){
    std::vector<long long> a(n,1);
    soryuusi::SplayTree<long long,add,zero> seq(a);
    for(int i=0;i<q;++i){int l=rng()%n,r=rng()%n;if(l>r)std::swap(l,r);++r;if(i&1)checksum+=seq.prod(l,r);else seq.flip(l,r);}
  }else if(mode == "lazy_splay"){
    std::vector<S> a(n,S{1,1});
    soryuusi::LazySplayTree<S,op,e,long long,mapping,add,zero> seq(a);
    for(int i=0;i<q;++i){int l=rng()%n,r=rng()%n;if(l>r)std::swap(l,r);++r;if(i&1)checksum+=seq.prod(l,r).first;else seq.apply(l,r,1);}
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

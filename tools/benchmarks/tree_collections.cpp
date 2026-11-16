#include <chrono>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "library/data_structure/splay_tree.hpp"
#include "library/data_structure/treap.hpp"
#include "library/data_structure/splay_explicit.hpp"
#include "library/data_structure/treap_explicit.hpp"

long long op(long long a,long long b){return a+b;}
long long e(){return 0;}

template<class Manager> long long sequences(){
#ifdef LEGACY_SEQUENCE_API
  using Sequence=Manager;
#else
  Manager arena;
  using Sequence=typename Manager::Sequence;
#endif
  constexpr int n=32768;
  std::vector<Sequence> sequence;
  sequence.reserve(n);
  for(int i=0;i<n;++i){
    std::vector<long long> values(4,i);
#ifdef LEGACY_SEQUENCE_API
    sequence.emplace_back(values);
#else
    sequence.push_back(arena.new_sequence(values));
#endif
  }
  for(int width=1;width<n;width*=2)for(int i=0;i<n;i+=width*2)sequence[i].concat(std::move(sequence[i+width]));
  return sequence[0].prod_all();
}
#ifndef LEGACY_SEQUENCE_API
template<class Manager> long long sets(){
  Manager arena;
  auto a=arena.new_set(),b=arena.new_set();
  constexpr int n=100000;
  for(int i=0;i<n;++i){a.insert(2*i,i);b.insert(2*i+1,i);}
  a.meld(std::move(b));
  for(int i=0;i<n;++i)a.erase(2*i);
  return a.prod_all();
}
#endif
int main(int argc,char** argv){
  std::string mode=argc>1 ? argv[1] : "splay";
  auto start=std::chrono::steady_clock::now();
  long long checksum;
  if(mode=="splay")checksum=sequences<soryuusi::SplayTree<long long,op,e>>();
  else if(mode=="treap")checksum=sequences<soryuusi::TreapSequence<long long,op,e>>();
#ifndef LEGACY_SEQUENCE_API
  else if(mode=="splay_explicit")checksum=sets<soryuusi::SplayExplicit<int,long long,op,e>>();
  else if(mode=="treap_explicit")checksum=sets<soryuusi::TreapExplicit<int,long long,op,e>>();
#endif
  else return 1;
  auto end=std::chrono::steady_clock::now();
  std::cout<<std::chrono::duration<double>(end-start).count()<<' '<<checksum<<'\n';
}

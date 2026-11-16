#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "library/data_structure/splay_explicit.hpp"
#include "library/data_structure/treap_explicit.hpp"
#include "library/data_structure/splay_tree.hpp"
#include "library/data_structure/treap.hpp"

namespace {
using namespace soryuusi;
using S = std::pair<long long,int>;
using F = std::pair<long long,long long>;
S op(S a,S b){return {a.first+b.first,a.second+b.second};}
S e(){return {0,0};}
S mapping(F f,S x){return {f.first*x.first+f.second*x.second,x.second};}
F composition(F f,F g){return {f.first*g.first,f.first*g.second+f.second};}
F id(){return {1,0};}
using Entry = std::pair<int,S>;
std::string concat(std::string a,std::string b){return a+b;}
std::string identity_string(){return {};}
long long add(long long a,long long b){return a+b;}
long long zero(){return 0;}

template<bool Multi> void normalize(std::vector<Entry>& a){
  std::stable_sort(a.begin(),a.end(),[](const auto& x,const auto& y){return x.first<y.first;});
  if constexpr(!Multi) a.erase(std::unique(a.begin(),a.end(),[](const auto& x,const auto& y){return x.first==y.first;}),a.end());
}
int lower(const std::vector<Entry>& a,int key){return std::lower_bound(a.begin(),a.end(),key,[](const auto& p,int k){return p.first<k;})-a.begin();}
int upper(const std::vector<Entry>& a,int key){return std::upper_bound(a.begin(),a.end(),key,[](int k,const auto& p){return k<p.first;})-a.begin();}
S sum(const std::vector<Entry>& a,int l,int r){S result=e();for(int i=l;i<r;++i)result=op(result,a[i].second);return result;}

template<class Manager,bool Multi> void random_sets(){
  Manager arena;
  using Set=typename Manager::Set;
  static_assert(!std::is_default_constructible_v<Set> && !std::is_copy_constructible_v<Set> && std::is_move_constructible_v<Set>);
  static_assert(Set::allows_duplicates==Multi);
  std::vector<Set> sets;
  std::vector<std::vector<Entry>> expected(7);
  for(int i=0;i<7;++i)sets.push_back(arena.new_set());
  assert(arena.allocated_nodes()==0);
  std::mt19937 rng(99823);
  for(int q=0;q<10000;++q){
    int k=rng()%sets.size(),action=rng()%10,key=(int)(rng()%41)-20;
    auto& a=expected[k];auto& set=sets[k];
    int l=lower(a,key),r=upper(a,key),n=a.size();
    if(action==0){
      S value{(long long)(rng()%100),1};bool inserted=set.insert(key,value);
      if(Multi || l==r){assert(inserted);a.insert(a.begin()+r,{key,value});}else assert(!inserted);
    }else if(action==1){bool erased=set.erase(key);assert(erased==(l<r));if(l<r)a.erase(a.begin()+l);}
    else if(action==2){assert(set.erase_all(key)==r-l);a.erase(a.begin()+l,a.begin()+r);}
    else if(action==3){
      int x=rng()%(n+1),y=rng()%(n+1);if(x>y)std::swap(x,y);
      F f{(long long)(rng()%2),(long long)(rng()%9)};set.apply(x,y,f);for(int i=x;i<y;++i)a[i].second=mapping(f,a[i].second);
    }else if(action==4){
      int high=key+rng()%20;F f{(long long)(rng()%2),(long long)(rng()%10)};
      set.apply_key(key,high,f);int end=lower(a,high);for(int i=l;i<end;++i)a[i].second=mapping(f,a[i].second);
    }else if(action==5){
      int high=key+rng()%20;assert(set.prod_key(key,high)==sum(a,l,lower(a,high)));
      int x=rng()%(n+1),y=rng()%(n+1);if(x>y)std::swap(x,y);assert(set.prod(x,y)==sum(a,x,y));
    }else if(action==6 && n){int p=rng()%n;assert(set.kth(p)==a[p]);S value{(long long)(rng()%100),1};set.set_at(p,value);a[p].second=value;}
    else if(action==7){
      int j=rng()%sets.size();if(j==k)continue;
      std::vector<Entry> combined=a;combined.insert(combined.end(),expected[j].begin(),expected[j].end());normalize<Multi>(combined);
      set.meld(std::move(sets[j]));assert(sets[j].empty());a=std::move(combined);expected[j].clear();
      sets[j].insert(key,S{7,1});expected[j].push_back({key,{7,1}});
    }else if(action==8){
      auto old=a;auto [left,right]=set.split_key(key);assert(set.empty() && left.size()==l && right.size()==n-l);
      left.meld(std::move(right));set=std::move(left);assert(set.to_vector()==old);
    }else if(action==9 && n){int p=rng()%n;set.erase_at(p);a.erase(a.begin()+p);}
    assert(set.size()==(int)a.size() && set.to_vector()==a && set.prod_all()==sum(a,0,a.size()));
    assert(set.lower_bound(key)==lower(a,key) && set.upper_bound(key)==upper(a,key));
    assert(set.count(key)==upper(a,key)-lower(a,key));
  }
  auto copy=sets[0].clone();auto original=sets[0].to_vector();copy.insert(100,S{5,1});assert(sets[0].to_vector()==original);
  Manager foreign;
  auto source=foreign.new_set(std::vector<Entry>{{-50,{1,1}},{0,{20,1}},{50,{3,1}},{0,{30,1}}});
  auto combined=expected[0];auto tail=source.to_vector();combined.insert(combined.end(),tail.begin(),tail.end());normalize<Multi>(combined);
  sets[0].meld(std::move(source));assert(source.empty() && sets[0].to_vector()==combined);
  auto survivor=[&](){Manager local;auto result=local.new_set(std::vector<Entry>{{1,{11,1}},{2,{22,1}}});return result;}();
  assert(survivor.kth(1)==Entry(2,S(22,1)));
  auto nodes=arena.allocated_nodes();
  auto recycle=arena.new_set();for(int i=0;i<1000;++i){recycle.insert(i,S{1,1});assert(recycle.erase(i));}
  assert(arena.allocated_nodes()<=nodes+1);
  Manager moved(std::move(arena));assert(sets[0].to_vector()==combined);auto empty=moved.new_set();assert(empty.empty());
}

struct Direction {
  bool descending;
  Direction()=delete;
  explicit Direction(bool value):descending(value){}
  bool operator()(int a,int b) const {return descending ? a>b : a<b;}
};
template<class Manager> void pending_meld(){
  Manager arena;
  auto a=arena.new_set(),b=arena.new_set();
  std::vector<Entry> expected;
  for(int i=0;i<200;++i){a.insert(i%50,S{i,1});b.insert(i%50,S{1000+i,1});}
  a.apply(0,a.size(),F{2,3});b.apply_key(10,40,F{0,7});
  for(int key=0;key<50;++key){
    for(int i=key;i<200;i+=50)expected.push_back({key,S{2*i+3,1}});
    for(int i=key;i<200;i+=50)expected.push_back({key,S{10<=key && key<40 ? 7 : 1000+i,1}});
  }
  a.meld(std::move(b));assert(b.empty() && a.to_vector()==expected);
}

template<template<class,class,auto,auto,bool,class> class Manager> void noncommutative(){
  Manager<int,std::string,concat,identity_string,true,std::less<int>> arena;
  auto a=arena.new_set();auto b=arena.new_set();
  a.insert(2,"a");a.insert(1,"L");a.insert(2,"b");b.insert(2,"c");b.insert(3,"R");b.insert(2,"d");
  a.meld(std::move(b));assert(a.prod_all()=="LabcdR" && a.prodrev_all()=="RdcbaL");
  assert(a.prod_key(2,3)=="abcd");
  Manager<int,std::string,concat,identity_string,false,std::less<int>> unique;
  auto x=unique.new_set();auto y=unique.new_set();x.insert(2,"old");y.insert(2,"new");x.meld(std::move(y));assert(x.prod_all()=="old");
  Manager<int,long long,add,zero,true,Direction> ascending(Direction{false}),descending(Direction{true});
  auto p=ascending.new_set();auto q=descending.new_set();q.insert(1,10);q.insert(3,30);q.insert(2,20);p.meld(std::move(q));
  assert((p.to_vector()==std::vector<std::pair<int,long long>>({{1,10},{2,20},{3,30}})));
}

struct NonDefaultKey {
  int value;
  NonDefaultKey()=delete;
  explicit NonDefaultKey(int x):value(x){}
  friend bool operator<(const NonDefaultKey& a,const NonDefaultKey& b){return a.value<b.value;}
};
void generic_keys(){
  TreapExplicit<NonDefaultKey,long long,add,zero> arena;
  auto set=arena.new_set();set.insert(NonDefaultKey(3),7);set.insert(NonDefaultKey(1),5);
  auto copy=set.clone();assert(copy.kth(0).first.value==1 && copy.prod_all()==12);
}

template<class Manager> void sequence_arena(){
  Manager arena;
  using Sequence=typename Manager::Sequence;
  static_assert(!std::is_default_constructible_v<Sequence>);
  std::vector<Sequence> sequences;
  for(int i=0;i<2000;++i)sequences.push_back(arena.new_sequence());
  assert(arena.allocated_nodes()==0);
  for(int i=0;i<2000;++i)sequences[i].insert_at(0,i);
  for(int i=1;i<2000;++i)sequences[0].concat(std::move(sequences[i]));
  assert(arena.allocated_nodes()==2000 && sequences[0].size()==2000);
  auto copied=sequences[0].clone();copied.set(0,-100);assert(sequences[0].get(0)==0);
  auto moved=std::move(arena);auto another=moved.new_sequence({3,4});assert(another.prod_all()==7);
  assert(sequences[0].get(1999)==1999);
  auto survivor=[](){Manager local;return local.new_sequence({10,20});}();assert(survivor.prod_all()==30);
  sequences[0].concat(std::move(survivor));assert(survivor.empty() && sequences[0].get(2001)==20);
}
}
int main(){
  random_sets<LazySplayExplicit<int,S,op,e,F,mapping,composition,id,true>,true>();
  random_sets<LazySplayExplicit<int,S,op,e,F,mapping,composition,id,false>,false>();
  random_sets<LazyTreapExplicit<int,S,op,e,F,mapping,composition,id,true>,true>();
  random_sets<LazyTreapExplicit<int,S,op,e,F,mapping,composition,id,false>,false>();
  pending_meld<LazySplayExplicit<int,S,op,e,F,mapping,composition,id>>();
  pending_meld<LazyTreapExplicit<int,S,op,e,F,mapping,composition,id>>();
  generic_keys();
  noncommutative<SplayExplicit>();noncommutative<TreapExplicit>();
  sequence_arena<SplayTree<long long,add,zero>>();sequence_arena<TreapSequence<long long,add,zero>>();
  std::cout<<"ordered_trees: all tests passed\n";
}

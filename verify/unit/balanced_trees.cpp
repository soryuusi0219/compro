#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include "library/data_structure/cartesian_tree.hpp"
#include "library/data_structure/dynamic_bit_vector.hpp"
#include "library/data_structure/dynamic_wavelet.hpp"
#include "library/data_structure/internal/binary_tree_ops.hpp"
#include "library/data_structure/internal/reversible_monoid.hpp"
#include "library/data_structure/lazy_link_cut.hpp"
#include "library/data_structure/link_cut.hpp"
#include "library/data_structure/splay_tree.hpp"
#include "library/data_structure/treap.hpp"

namespace {
using namespace soryuusi;
using S = std::pair<long long,int>;
using F = std::pair<long long,long long>;
S op(S a, S b){return {a.first+b.first,a.second+b.second};}
S e(){return {0,0};}
S mapping(F f, S x){return {f.first*x.first+f.second*x.second,x.second};}
F composition(F f,F g){return {f.first*g.first,f.first*g.second+f.second};}
F id(){return {1,0};}
std::string concatenate(std::string a, std::string b){return a+b;}
std::string empty_string(){return {};}

void bit_vectors(){
  std::mt19937 rng(823746);
  for(int n : {0,1,2,31,32,33,47,48,49,63,64,65,95,96,97,127,128,129,1000}){
    std::vector<bool> values(n);
    for(int i = 0; i < n; ++i) values[i] = rng()&1;
    DynamicBitVector bits(values);
    assert(bits.size() == n && bits.to_vector() == values);
    int ones = 0;
    for(int i = 0; i <= n; ++i){assert(bits.rank(i) == ones); if(i<n) ones += values[i];}
    for(int q = 0; q < 2500; ++q){
      int len = values.size(), action = rng()%5;
      if(!len || action == 0){int i = rng()%(len+1); bool b = rng()&1; values.insert(values.begin()+i,b); bits.insert(i,b);}
      else if(action == 1){int i = rng()%len; values.erase(values.begin()+i); bits.erase(i);}
      else if(action == 2){int i = rng()%len; bool b = rng()&1; values[i] = b; bits.set(i,b);}
      else if(action == 3){int l = rng()%(len+1), r = rng()%(len+1); if(l>r) std::swap(l,r); assert(bits.rank(l,r) == std::count(values.begin()+l,values.begin()+r,true));}
      else{int i = rng()%len; assert(bits.get(i) == values[i]);}
      assert(bits.size() == (int)values.size());
      if(q%100 == 0) assert(bits.to_vector() == values);
    }
    auto copy = bits.clone(); assert(copy.to_vector() == values);
    if(!values.empty()){copy.set(0,!values[0]); assert(bits.get(0) == values[0]);}
    DynamicBitVector moved(std::move(bits)); assert(bits.empty());
    bits.insert(0,true); assert(bits.size() == 1 && bits.rank(1) == 1);
    bits = std::move(moved); assert(moved.empty() && bits.to_vector() == values);
    while(!values.empty()){bits.erase(0); values.erase(values.begin());}
    assert(bits.empty() && bits.rank(0) == 0);
    auto count = bits.allocated_nodes();
    for(int q = 0; q < 1000; ++q){bits.insert(0,q&1); bits.erase(0);}
    assert(bits.allocated_nodes() <= std::max<std::size_t>(count,1));
    bits.build({true,false,true}); assert(bits.size() == 3 && bits.rank(3) == 2);
    bits.build({}); assert(bits.empty());
  }
}

void bit_block_updates(){
  for(bool bit : {false,true}){
    DynamicBitVector bits(std::vector<bool>(257,bit));
    for(int i = 0; i < 400; ++i) bits.insert(bits.size(),bit);
    for(int i = 0; i < 400; ++i) bits.insert(0,bit);
    auto allocated = bits.allocated_nodes();
    for(int k : {0,1,31,32,47,48,63,64,65,127,128,129,1056,1057}){
      assert(bits.rank(k) == (bit ? k : 0));
      assert(bits.rank(k,bits.size()) == (bit ? bits.size()-k : 0));
    }
    assert(bits.allocated_nodes() == allocated);
    for(int i = 0; i < 400; ++i) bits.erase(bits.size()-1);
    for(int i = 0; i < 400; ++i) bits.erase(0);
    for(int i = 0; i < 400; ++i) bits.insert(bits.size(),!bit);
    assert(bits.allocated_nodes() == allocated && bits.rank(257,657) == (bit ? 0 : 400));

    for(int i = bits.size()-1; i >= 0; --i) if(i%47) bits.erase(i);
    auto copy = bits.clone();
    assert(bits.size() == 14 && copy.to_vector() == bits.to_vector());
    copy.set(0,!bits.get(0));
    assert(bits.get(0) != copy.get(0));
  }
}

template<class Manager> void sequence_test(){
  Manager arena;
  using Sequence = typename Manager::Sequence;
  static_assert(!std::is_copy_constructible_v<Sequence> && std::is_move_assignable_v<Sequence>);
  std::mt19937 rng(1459);
  std::vector<S> a(300);
  for(int i = 0; i < (int)a.size(); ++i) a[i] = {i,1};
  auto seq = arena.new_sequence(a);
  for(int q = 0; q < 6000; ++q){
    int n = a.size(), action = rng()%8;
    int l = rng()%(n+1), r = rng()%(n+1); if(l>r) std::swap(l,r);
    if(action == 0 || !n){S x{(long long)(rng()%100),1}; seq.insert_at(l,x); a.insert(a.begin()+l,x);}
    else if(action == 1){int i = rng()%n; seq.erase_at(i); a.erase(a.begin()+i);}
    else if(action == 2){seq.flip(l,r); std::reverse(a.begin()+l,a.begin()+r);}
    else if(action == 3){F f{(long long)(rng()%2),(long long)(rng()%11)}; seq.apply(l,r,f); for(int i=l;i<r;++i) a[i] = mapping(f,a[i]);}
    else if(action == 4){int i=rng()%n; S x{(long long)(rng()%100),1}; seq.set(i,x); a[i]=x; assert(seq.get(i)==x);}
    else if(action == 5){auto [left,right] = seq.split(l); assert(seq.empty()); left.concat(std::move(right)); assert(right.empty()); seq=std::move(left);}
    else if(action == 6){seq.flip_all(); std::reverse(a.begin(),a.end());}
    else{S sum=e();for(int i=l;i<r;++i)sum=op(sum,a[i]);assert(seq.prod(l,r)==sum && seq.prodrev(l,r)==sum);}
    assert(seq.size() == (int)a.size());
    if(q%53 == 0) assert(seq.to_vector() == a);
  }
  Sequence independent = seq.clone(); independent.insert_at(0,{999,1});
  assert(seq.to_vector() == a && independent.size() == seq.size()+1);
  auto survivor = arena.new_sequence();
  {Manager local_arena; auto local = local_arena.new_sequence(a); auto [left,right] = local.split(a.size()/2); left.concat(std::move(right)); survivor = std::move(left);}
  assert(survivor.to_vector() == a);
  {Manager donor_arena; auto donor = donor_arena.new_sequence(std::vector<S>{{7,1},{8,1}}); survivor.concat(std::move(donor)); assert(donor.empty());}
  a.push_back({7,1});a.push_back({8,1}); assert(survivor.to_vector() == a);
  survivor.erase_at(a.size()-1);a.pop_back();assert(survivor.to_vector()==a);
}

template<class Manager> void noncommutative_sequence(){
  Manager arena;
  auto sequence = arena.new_sequence(std::vector<std::string>{"a","b","c","d","e"});
  assert(sequence.prod(1,4)=="bcd" && sequence.prodrev(1,4)=="dcb");
  sequence.flip(1,5); assert(sequence.prod_all()=="aedcb");
  sequence.set(2,"x"); assert(sequence.prod_all()=="aexcb" && sequence.prodrev_all()=="bcxea");
  auto [left,right]=sequence.split(2);right.flip_all();left.concat(std::move(right));assert(left.prod_all()=="aebcx");
}

template<class Manager> void split_pending_sequence(){
  Manager arena;
  auto seq = arena.new_sequence(std::vector<S>{{0,1},{1,1},{2,1},{3,1},{4,1}});
  auto ref = seq.ref_at(1);
  seq.apply(0,5,{2,1});
  seq.flip_all();
  auto [left,right] = seq.split(2);
  assert(left.prod(0,1) == S(9,1) && right.prodrev(1,2) == S(3,1));
  assert(left.index_of(ref) == -1 && right.index_of(ref) == 1);
  left.concat(std::move(right));
  assert(right.empty() && left.index_of(ref) == 3);
  assert(left.to_vector() == std::vector<S>({{9,1},{7,1},{5,1},{3,1},{1,1}}));
}

void cartesian(){
  auto [empty_parent,empty_left,empty_right,empty_root] = cartesian_tree(std::vector<int>{});
  assert(empty_parent.empty() && empty_left.empty() && empty_right.empty() && empty_root == 0);
  auto [min_parent,min_left,min_right,min_root] = cartesian_tree(std::vector<int>{1,1,1});
  auto [max_parent,max_left,max_right,max_root] = cartesian_tree(std::vector<int>{1,1,1},true);
  assert(min_root == 2 && min_parent == std::vector<int>({1,2,-1}));
  assert(min_left == std::vector<int>({-1,0,1}) && min_right == std::vector<int>({-1,-1,-1}));
  assert(max_root == 0 && max_parent == std::vector<int>({-1,0,1}));
  assert(max_left == std::vector<int>({-1,-1,-1}) && max_right == std::vector<int>({1,2,-1}));

  std::mt19937 rng(517);
  for(bool maximum : {false,true}) for(int n=1;n<100;++n){
    std::vector<int> a(n);for(auto& x:a)x=rng()%8;
    auto [parent,left,right,root]=cartesian_tree(a,maximum);
    std::vector<int> inorder;
    auto dfs=[&](auto&& self,int i)->void{
      if(i<0)return;
      for(int child:{left[i],right[i]})if(child>=0){assert(parent[child]==i);assert(maximum ? a[i]>=a[child] : a[i]<=a[child]);}
      self(self,left[i]);inorder.push_back(i);self(self,right[i]);
    };
    assert(parent[root]==-1);dfs(dfs,root);
    for(int i=0;i<n;++i)assert(inorder[i]==i);
  }
}

void wavelets(){
  std::mt19937 rng(19286);
  std::vector<int> a(67);for(auto& x:a)x=rng()%256;
  DynamicWavelet<int,8> wavelet(a);
  for(int q=0;q<2500;++q){
    int n=a.size(),action=rng()%4;
    if(!n || action==0){int i=rng()%(n+1),x=rng()%256;wavelet.insert(i,x);a.insert(a.begin()+i,x);}
    else if(action==1){int i=rng()%n;wavelet.erase(i);a.erase(a.begin()+i);}
    else if(action==2){int i=rng()%n,x=rng()%256;wavelet.set(i,x);a[i]=x;}
    else{int l=rng()%(n+1),r=rng()%(n+1),x=rng()%256;if(l>r)std::swap(l,r);
      assert(wavelet.rank(l,r,x)==std::count(a.begin()+l,a.begin()+r,x));
      assert(wavelet.count_lt(l,r,x)==std::count_if(a.begin()+l,a.begin()+r,[&](int y){return y<x;}));
      if(l<r){std::vector<int> sorted(a.begin()+l,a.begin()+r);std::sort(sorted.begin(),sorted.end());int k=rng()%sorted.size();assert(wavelet.range_kth(l,r,k)==sorted[k]);}
    }
    if(q%41==0)for(int i=0;i<(int)a.size();++i)assert(wavelet.get(i)==a[i]);
  }
}

void wavelet_boundaries(){
  std::vector<unsigned> values(63,0);
  DynamicWavelet<unsigned,8> wavelet(values);
  // The same position grows across several levels while all elements are equal.
  for(int i = 0; i < 130; ++i){
    wavelet.insert(wavelet.size(),0);
    values.push_back(0);
  }
  for(int i : {0,31,47,48,63,64,127,128,192}){
    wavelet.set(i,255);
    values[i] = 255;
    wavelet.insert(i,1);
    values.insert(values.begin()+i,1);
    wavelet.set(i,0);
    values[i] = 0;
    wavelet.erase(i+1);
    values.erase(values.begin()+i+1);
  }
  wavelet.set(64,128); values[64] = 128;
  wavelet.set(48,1); values[48] = 1;
  int n = values.size();
  for(int i = 0; i < n; ++i) assert(wavelet.get(i) == values[i]);
  assert(wavelet.rank(n,n,0) == 0 && wavelet.count_lt(0,n,255) == n);
  assert(wavelet.count_interval(0,n,1,129) == 2);
  assert(wavelet.next(0,n,2,999) == 128 && wavelet.prev(0,n,128,999) == 1);
  assert(wavelet.next(0,n,255,999) == 999 && wavelet.prev(n,n,255,999) == 999);
  auto copy = wavelet.clone();
  for(int i = 0; i < n; ++i) assert(copy.get(i) == values[i]);
  auto moved = std::move(wavelet);
  assert(wavelet.size() == 0);
  wavelet.insert(0,13);
  assert(wavelet.get(0) == 13 && moved.range_kth(0,n,n-1) == 128);
  for(int i = 0; i < n; ++i) moved.erase(0);
  assert(moved.size() == 0 && moved.count_lt(0,0,1) == 0);

  using U = std::uint64_t;
  constexpr U high = U{1}<<63, largest = std::numeric_limits<U>::max();
  DynamicWavelet<U,64> full({0,largest,high,1});
  full.set(0,high+1);
  full.set(3,largest);
  assert(full.get(0) == high+1 && full.rank(0,4,largest) == 2);
  assert(full.count_lt(0,4,largest) == 2 && full.range_kth(0,4,0) == high);
}

std::vector<int> path(const std::vector<std::vector<int>>& graph,int s,int t){
  std::vector<int> parent(graph.size(),-1);std::queue<int> queue;parent[s]=s;queue.push(s);
  while(!queue.empty()){int u=queue.front();queue.pop();for(int v:graph[u])if(parent[v]<0){parent[v]=u;queue.push(v);}}
  if(parent[t]<0)return {};
  std::vector<int> p;for(int u=t;;u=parent[u]){p.push_back(u);if(u==s)break;}
  std::reverse(p.begin(),p.end());return p;
}
void forests(){
  constexpr int n=35;
  lazy_linkcuttree<S,op,e,F,mapping,composition,id> forest(n,S{0,1});
  linkcuttree<std::string,concatenate,empty_string> words(n);
  std::vector<std::vector<int>> graph(n);
  std::vector<long long> values(n);std::vector<std::string> letters(n);
  for(int i=0;i<n;++i){letters[i]=char('a'+i%26);words.set(i,letters[i]);}
  std::mt19937 rng(9915);
  for(int q=0;q<8000;++q){
    int u=rng()%n,v=rng()%n,action=rng()%6;auto p=path(graph,u,v);
    assert(forest.connected(u,v)==!p.empty() && words.connected(u,v)==!p.empty());
    if(action==0 && p.empty()){
      graph[u].push_back(v);graph[v].push_back(u);forest.link(u,v);words.link(u,v);
    }else if(action==1 && !graph[u].empty()){
      v=graph[u][rng()%graph[u].size()];forest.cut(u,v);words.cut(u,v);
      graph[u].erase(std::find(graph[u].begin(),graph[u].end(),v));graph[v].erase(std::find(graph[v].begin(),graph[v].end(),u));
    }else if(action==2){values[u]=rng()%100;forest.set(u,{values[u],1});assert(forest.get(u).first==values[u]);}
    else if(action==3 && !p.empty()){
      F f{(long long)(rng()%2),(long long)(rng()%10)};forest.apply(u,v,f);for(int i:p)values[i]=f.first*values[i]+f.second;
    }else if(!p.empty()){
      long long sum=0;std::string text;for(int i:p){sum+=values[i];text+=letters[i];}
      assert(forest.prod(u,v)==S(sum,p.size()));assert(words.prod(u,v)==text);
      assert(forest.get_path(u,v)==p && words.get_path(u,v)==p);
      forest.evert(u);words.evert(u);assert(forest.find_root(v)==u && words.find_root(v)==u);
      int z=p[p.size()/2];assert(forest.lca(z,v)==z && words.lca(z,v)==z);
    }
  }
  auto copy=forest.clone();for(int i=0;i<n;++i)assert(copy.get(i)==forest.get(i));
}

void preferred_paths(){
  constexpr int n = 4097, mid = n/2;
  lazy_linkcuttree<S,op,e,F,mapping,composition,id> chain(n,S{1,1});
  for(int i = 1; i < n; ++i) chain.link(i-1,i);
  chain.apply(n-1,0,{2,1});
  chain.evert(n-1);
  assert(chain.get(mid) == S(3,1));
  chain.set(mid,{12,1});
  assert(chain.prod(0) == S(3*n+9,n) && chain.lca(0,mid) == mid);
  auto forward = chain.get_path(0,n-1);
  assert(forward.size() == n);
  for(int i = 0; i < n; ++i) assert(forward[i] == i);
  chain.cut(mid,mid+1);
  assert(!chain.connected(0,n-1) && chain.get(mid+1) == S(3,1));
  chain.link(0,n-1);
  assert(chain.get_path(1,n-2) == std::vector<int>({1,0,n-1,n-2}));
  assert(chain.prod(1,n-2) == S(12,4) && chain.get(mid) == S(12,1));

  linkcuttree<std::string,concatenate,empty_string> star(5);
  for(int i = 0; i < 5; ++i) star.set(i,std::string(1,'a'+i));
  for(int i = 1; i < 5; ++i) star.link(0,i);
  for(int i = 1; i < 5; ++i){
    assert(star.prod(i,0) == std::string(1,'a'+i)+"a");
    assert(star.prod(0,i) == "a"+std::string(1,'a'+i));
    assert(star.lca(i%4+1,(i+1)%4+1) == 0);
  }
}
}

int main(){
  bit_vectors();
  bit_block_updates();
  sequence_test<LazySplayTree<S,op,e,F,mapping,composition,id>>();
  sequence_test<LazyTreapSequence<S,op,e,F,mapping,composition,id>>();
  split_pending_sequence<LazySplayTree<S,op,e,F,mapping,composition,id>>();
  split_pending_sequence<LazyTreapSequence<S,op,e,F,mapping,composition,id>>();
  noncommutative_sequence<SplayTree<std::string,concatenate,empty_string>>();
  noncommutative_sequence<TreapSequence<std::string,concatenate,empty_string>>();
  cartesian();wavelets();wavelet_boundaries();forests();preferred_paths();
  std::cout << "balanced_trees: all tests passed\n";
}

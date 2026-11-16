#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "library/data_structure/binary_trie.hpp"
#include "library/data_structure/dynamic_lazy_segtree.hpp"
#include "library/data_structure/dynamic_segtree.hpp"
#include "library/data_structure/dynamic_segtree_2d.hpp"
#include "library/data_structure/dynamic_wavelet.hpp"
#include "library/data_structure/lazy_link_cut.hpp"
#include "library/data_structure/li_chao_tree_dynamic.hpp"
#include "library/data_structure/link_cut.hpp"
#include "library/data_structure/persistent_dsu.hpp"
#include "library/data_structure/persistent_dsu_with_data.hpp"
#include "library/data_structure/persistent_dynamic_lazy_segtree.hpp"
#include "library/data_structure/persistent_dynamic_segtree.hpp"
#include "library/data_structure/persistent_queue.hpp"
#include "library/data_structure/persistent_vector.hpp"
#include "library/data_structure/point_set_range_count.hpp"
#include "library/data_structure/splay_tree.hpp"
#include "library/data_structure/splay_tree_lazy.hpp"

namespace {
using namespace soryuusi;
long long add(long long a, long long b){return a+b;}
long long zero(){return 0;}
using S = std::pair<long long,long long>;
S op(S a, S b){return {a.first+b.first,a.second+b.second};}
S e(){return {0,0};}
using F = std::pair<long long,long long>;
S mapping(F f, S x){return {f.first*x.first+f.second*x.second,x.second};}
F composition(F f, F g){return {f.first*g.first,f.first*g.second+f.second};}
F id(){return {1,0};}
using Lazy = Dynamic_LazySegtree<S,op,e,F,mapping,composition,id>;
using PersistentLazy = Persistent_Dynamic_LazySegtree<S,op,e,F,mapping,composition,id>;
using Seg = Dynamic_Segtree<long long,add,zero>;
using PS = PersistentDynamicSegtree<long long,add,zero>;
using Seq = SplayTree<long long,add,zero>;
using LazySeq = LazySplayTree<S,op,e,F,mapping,composition,id>;
using LCT = linkcuttree<long long,add,zero>;
using LazyLCT = lazy_linkcuttree<S,op,e,F,mapping,composition,id>;
using LiChao = DynamicLiChaoTree<long long,-64,64,1,1000000000LL>;
using Count = PointSetRangeCount<int,hash_int<int>>;
using Trie = Binary_Trie<unsigned,8>;
using Wavelet = DynamicWavelet<int,8>;

template<class T> constexpr bool move_only = !std::is_copy_constructible_v<T> &&
    !std::is_copy_assignable_v<T> && std::is_move_constructible_v<T> && std::is_move_assignable_v<T>;
static_assert(move_only<Seg> && move_only<Lazy> && move_only<Dynamic_Segtree2D<long long,add,zero>>);
static_assert(move_only<Count> && move_only<LiChao> && move_only<Trie> && move_only<Wavelet>);
static_assert(move_only<Seq> && move_only<LazySeq> && move_only<LCT> && move_only<LazyLCT>);
static_assert(move_only<PS> && move_only<PersistentLazy> && move_only<Persistent_Vector<int>>);
static_assert(move_only<PersistentDSU> && move_only<PersistentDSUWithData<long long,add>> && move_only<Persistent_Queue<int>>);
static_assert(!std::is_pointer_v<PS::Root>);

void mutable_trees(){
  Seg a(1000000000); a.set(4,3);
  auto b = a.clone(); b.set(4,9);
  assert(a.get(4) == 3 && b.get(4) == 9);
  Seg c(std::move(a)); assert(c.get(4) == 3 && a.all_prod() == 0);
  a.set(5,7); c = std::move(a); assert(c.get(5) == 7 && a.all_prod() == 0);
  Lazy l(13,[](int i,int j){return S{0,j-i};});
  l.apply(0,13,{2,3}); l.apply(2,11,{3,1});
  auto allocated = l.allocated_nodes();
  assert(l.get(4).first == 10 && l.prod(1,5).first == 33);
  assert(l.allocated_nodes() == allocated);
  auto lc = l.clone(); lc.set(3,{7,1}); assert(l.get(3).first == 10);
  Lazy lm(std::move(l)); assert(lm.get(4).first == 10 && l.all_prod() == S(0,13));
  Dynamic_Segtree2D<long long,add,zero> t(100,100); t.set(3,7,9);
  auto tc = t.clone(); tc.add(3,7,2); assert(t.get(3,7) == 9 && tc.get(3,7) == 11);
  auto tm = std::move(t); assert(tm.all_prod() == 9 && t.all_prod() == 0);
  Count counts(std::vector<int>{1,2,1,3},20); auto cc = counts.clone(); cc.set(0,2);
  assert(counts.count(0,4,1) == 2 && cc.count(0,4,1) == 1);
  auto cm = std::move(counts); assert(cm.count(0,4,1) == 2 && counts.count(0,0,1) == 0);
  LiChao lines; lines.add_line({1,3}); lines.add_segment({-2,5},-10,20);
  auto copied = lines.clone(); copied.add_line({0,-100}); assert(lines.get_min(7) == -9);
  LiChao source; source.add_segment({0,-50},-5,5); lines.merge(std::move(source));
  assert(lines.get_min(0) == -50 && lines.get_min(7) == -9 && source.get_min(0) == 1000000000LL);
  auto moved = std::move(lines); assert(moved.get_min(0) == -50);
  Trie trie; for(unsigned x : {3,17,18,128}) trie.insert(x);
  assert(trie.max_element() == 128); auto trc = trie.clone(); trc.operate_xor(255);
  Trie donor; donor.insert(4); donor.insert(17); trie.merge(std::move(donor));
  assert(trie.size() == 5 && donor.size() == 0); donor.insert(6); assert(donor.min_element() == 6);
  auto trm = std::move(trie); trie.insert(8); assert(trm.size() == 5 && trie.size() == 1);
  Wavelet w(std::vector<int>{1,5,3,9}); auto wc = w.clone(); wc.set(0,7);
  assert(w.get(0) == 1 && wc.get(0) == 7);
  auto wm = std::move(w); w.insert(0,4); assert(wm.get(1) == 5 && w.get(0) == 4);
}

void persistent_versions(){
  std::mt19937 rng(1234567);
  constexpr int n = 23;
  PersistentLazy seg(n,[](int l,int r){return S{0,r-l};});
  PS plain(n);
  Persistent_Vector<int> array;
  std::vector<PersistentLazy::Root> roots{seg.empty_root()};
  std::vector<PS::Root> proots{plain.empty_root()};
  std::vector<Persistent_Vector<int>::Root> aroots{array.empty_root()};
  std::vector<std::vector<long long>> expected(1,std::vector<long long>(n));
  for(int q = 0; q < 1200; ++q){
    int k = rng()%roots.size(), src = rng()%roots.size();
    int l = rng()%(n+1), r = rng()%(n+1); if(l > r) std::swap(l,r);
    auto values = expected[k]; auto next = roots[k];
    int action = rng()%4;
    F f{(long long)(rng()%2),(long long)(rng()%11)-5};
    if(action == 0){next = seg.apply(next,l,r,f); for(int i = l; i < r; ++i) values[i] = f.first*values[i]+f.second;}
    else if(action == 1){next = seg.copy_range(next,roots[src],l,r); for(int i = l; i < r; ++i) values[i] = expected[src][i];}
    else if(action == 2){next = seg.copy_range_with_apply(next,roots[src],l,r,f); for(int i = l; i < r; ++i) values[i] = f.first*expected[src][i]+f.second;}
    else{int p = rng()%n; values[p] = f.second; next = seg.set(next,p,{values[p],1});}
    roots.push_back(next); expected.push_back(values);
    auto count = seg.allocated_nodes(); auto old = roots[k];
    for(int j : {k,(int)roots.size()-1}){
      assert(seg.prod(roots[j],l,r).first == std::accumulate(expected[j].begin()+l,expected[j].begin()+r,0LL));
      for(int i = 0; i < n; ++i) assert(seg.get(roots[j],i).first == expected[j][i]);
    }
    assert(roots[k] == old && seg.allocated_nodes() == count);
    // Exercise independent point-update histories in the other persistent APIs.
    int p = rng()%n; int value = rng()%100;
    auto pr = plain.set(proots[k],p,value); auto ar = array.set(aroots[k],p,value);
    for(int i = 0; i < n; ++i){
      long long want = i == p ? value : plain.get(proots[k],i);
      assert(plain.get(pr,i) == want && array.get(ar,i) == want);
    }
    proots.push_back(pr); aroots.push_back(ar);
  }
  auto retained = roots.back(); auto moved = std::move(seg);
  assert(moved.all_prod(retained).first == std::accumulate(expected.back().begin(),expected.back().end(),0LL));
  PersistentDSU dsu(20); PersistentDSUWithData<long long,add> data(20,1);
  std::vector<PersistentDSU::Root> dr{dsu.empty_root()};
  std::vector<decltype(data)::Root> vr{data.empty_root()};
  std::vector<std::vector<int>> groups(1,std::vector<int>(20)); std::iota(groups[0].begin(),groups[0].end(),0);
  for(int q = 0; q < 800; ++q){
    int k = rng()%dr.size(), u = rng()%20, v = rng()%20;
    auto group = groups[k]; int from = group[u], to = group[v];
    for(auto& g : group) if(g == from) g = to;
    auto [leader, root] = dsu.merge(u,v,dr[k]); auto [dl, value_root] = data.merge(u,v,vr[k]);
    assert(leader == dsu.leader(root,u) && dl == data.leader(value_root,u));
    dr.push_back(root); vr.push_back(value_root); groups.push_back(group);
    auto dc = dsu.allocated_nodes(), vc = data.allocated_nodes();
    for(int i = 0; i < 20; ++i){
      assert(dsu.same(root,u,i) == (group[u] == group[i]));
      assert(dsu.same(dr[k],u,i) == (groups[k][u] == groups[k][i]));
      int size = std::count(group.begin(),group.end(),group[i]);
      assert(dsu.size(root,i) == size && data.size(value_root,i) == size && data.get(value_root,i) == size);
    }
    assert(dsu.allocated_nodes() == dc && data.allocated_nodes() == vc);
  }
  Persistent_Queue<int> queue;
  std::vector<decltype(queue)::Root> qr{queue.empty_root()}; std::vector<std::deque<int>> queues(1);
  assert(queue.empty(qr[0]) && queue.size(qr[0]) == 0);
  for(int q = 0; q < 2500; ++q){
    int k = rng()%qr.size(); auto values = queues[k]; auto old = qr[k];
    if(values.empty() || rng()%2){int x = rng(); qr.push_back(queue.push(qr[k],x)); values.push_back(x);}
    else{assert(queue.front(qr[k]) == values.front()); qr.push_back(queue.pop(qr[k])); values.pop_front();}
    queues.push_back(values);
    assert(qr[k] == old && queue.size(qr.back()) == (int)values.size());
    if(!values.empty()) assert(queue.front(qr.back()) == values.front());
  }
  // Long histories exercise all levels of the doubling table and empty normalization.
  auto qroot = queue.empty_root();
  for(int i = 0; i < 4096; ++i) qroot = queue.push(qroot,i);
  for(int i = 0; i < 4096; ++i){assert(queue.front(qroot) == i); qroot = queue.pop(qroot);}
  assert(queue.empty(qroot));
}

std::string concatenate(std::string a, std::string b){return a+b;}
std::string empty_string(){return {};}
void sequences(){
  std::mt19937 rng(42);
  LazySeq seq; std::vector<long long> values;
  for(int q = 0; q < 2000; ++q){
    int n = values.size(), action = rng()%6;
    if(!n || action == 0){int p = rng()%(n+1), x = rng()%10; seq.insert_at(p,{x,1}); values.insert(values.begin()+p,x);}
    else if(action == 1){int p = rng()%n; seq.erase_at(p); values.erase(values.begin()+p);}
    else{
      int l = rng()%(n+1), r = rng()%(n+1); if(l > r) std::swap(l,r);
      if(action == 2){seq.flip(l,r); std::reverse(values.begin()+l,values.begin()+r);}
      else if(action == 3){F f{(long long)(rng()%2),(long long)(rng()%10)}; seq.apply(l,r,f); for(int i = l; i < r; ++i) values[i] = f.first*values[i]+f.second;}
      else if(action == 4){assert(seq.prod(l,r).first == std::accumulate(values.begin()+l,values.begin()+r,0LL));}
      else{
        auto [a,b] = seq.split(l); assert(seq.empty() && a.size() == l && b.size() == n-l);
        a.concat(std::move(b)); assert(b.empty()); seq = std::move(a);
      }
    }
    auto actual = seq.to_vector(); assert(actual.size() == values.size());
    for(int i = 0; i < (int)values.size(); ++i) assert(actual[i] == S(values[i],1));
  }
  SplayTree<std::string,concatenate,empty_string> words(std::vector<std::string>{"a","b","c","d"});
  words.flip(1,4); assert(words.prod_all() == "adcb" && words.prodrev_all() == "bcda");
  words.flip_all(); assert(words.prod_all() == "bcda");
  Seq survivor;
  {
    Seq sequence(std::vector<long long>{1,2,3,4});
    auto [left,right] = sequence.split(2);
    assert(sequence.empty());
    left.concat(std::move(right));
    survivor = std::move(left);
  } // The original object and both split handles have been destroyed.
  {Seq foreign(std::vector<long long>{5,6}); survivor.concat(std::move(foreign)); assert(foreign.empty());}
  survivor.erase_at(4); assert(survivor.to_vector() == std::vector<long long>({1,2,3,4,6}));
  auto copy = survivor.clone(); copy.set(0,100); assert(survivor.get(0) == 1);
  auto [empty, tail] = survivor.split(0); empty.concat(std::move(tail)); assert(empty.size() == 5 && tail.empty());
}

void forests(){
  LCT forest(8);
  for(int i = 0; i < 8; ++i) forest.set(i,i+1);
  for(int i = 0; i < 6; ++i) forest.link(i,i+1);
  assert(forest.connected(0,6) && !forest.same(0,7) && forest.prod(0,6) == 28);
  auto copy = forest.clone(); copy.cut(2,3); assert(!copy.connected(0,6) && forest.connected(0,6));
  LCT moved(std::move(forest)); assert(forest.size() == 0 && moved.prod(0,6) == 28);
  moved.cut(4,5); moved.link(4,7); assert(moved.prod(0,7) == 23);
  LazyLCT lazy(8,S{0,1});
  for(int i = 0; i < 7; ++i) lazy.link(i,i+1);
  lazy.apply(0,7,{1,2}); lazy.apply(1,6,{3,1});
  assert(lazy.prod(0,7).first == 46);
  lazy.set(3,{100,1}); assert(lazy.get(3).first == 100 && lazy.prod(0,7).first == 139);
  auto lc = lazy.clone(); lc.cut(3,4); assert(!lc.connected(0,7) && lazy.connected(0,7));
  auto lm = std::move(lazy); assert(lm.prod(0,7).first == 139 && lazy.size() == 0);
}
}

int main(int argc, char** argv){
  if(argc == 2){
    std::string mode = argv[1];
    if(mode == "foreign-root") {PS a(8), b(8); auto root = a.set(a.empty_root(),1,2); return b.get(root,1);}
    if(mode == "replaced-root") {PS a(8); auto root = a.set(a.empty_root(),1,2); a = PS(8); return a.get(root,1);}
    if(mode == "cycle") {LCT a(3); a.link(0,1); a.link(1,2); a.link(0,2);}
    if(mode == "nonedge") {LCT a(3); a.link(0,1); a.link(1,2); a.cut(0,2);}
    return 0;
  }
  mutable_trees(); persistent_versions(); sequences(); forests();
  std::cout << "pointer_structures: all tests passed\n";
}

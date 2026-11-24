#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>
#include "library/data_structure/splay_sortable.hpp"
#include "library/data_structure/treap_sortable.hpp"

namespace {
using namespace soryuusi;
using Entry = std::pair<int,std::string>;
using Affine = std::pair<int,int>;
std::string op(std::string x,std::string y){return x+y;}
std::string identity(){return {};}
Affine affine_compose(Affine f,Affine g){return {f.first*g.first,f.first*g.second+f.second};}
Affine affine_id(){return {1,0};}
int map_key(Affine f,int key){return f.first*key+f.second;}
std::string map_value(int shift,std::string v){
  for(char& c : v) c = 'a'+(c-'a'+shift)%26;
  return v;
}
int compose_value(int f,int g){return (f+g)%26;}
int value_id(){return 0;}
std::string product(const std::vector<Entry>& v,int l,int r,bool reverse = false){
  std::string result;
  for(int i=l;i<r;++i) result += v[reverse ? r-1-(i-l) : i].second;
  return result;
}

template<class Manager,class Compare = std::less<int>>
void random_operations(Compare compare = {}){
  Manager arena(compare);
  auto seq = arena.new_sequence();
  std::vector<Entry> expected;
  std::mt19937 rng(981803);
  for(int q=0;q<2500;++q){
    const int n = expected.size();
    int l=rng()%(n+1),r=rng()%(n+1);
    if(l>r) std::swap(l,r);
    const bool descending = rng()%2;
    switch(rng()%12){
    case 0:
      if(n<64){
        Entry entry = {int(rng()%33)-16,std::string(1,'a'+rng()%26)};
        seq.insert_at(l,entry);
        expected.insert(expected.begin()+l,entry);
      }
      break;
    case 1:
      if(r-l>3) r=l+3;
      assert(seq.erase_range(l,r)==r-l);
      expected.erase(expected.begin()+l,expected.begin()+r);
      break;
    case 2:
      if(l<n){
        Entry entry = {int(rng()%33)-16,std::string(1,'a'+rng()%26)};
        seq.set(l,entry);
        expected[l]=entry;
      }
      break;
    case 3:
      if(l<n){
        seq.set_at(l,"j");
        expected[l].second="j";
      }
      break;
    case 4:
      assert(seq.prod(l,r)==product(expected,l,r));
      assert(seq.prodrev(l,r)==product(expected,l,r,true));
      break;
    case 5:{
      seq.sort(l,r,descending);
      auto actual=seq.to_vector();
      auto first=actual.begin()+l,last=actual.begin()+r;
      assert(std::is_sorted(first,last,[&](const Entry& a,const Entry& b){
        return descending ? compare(b.first,a.first) : compare(a.first,b.first);
      }));
      std::vector<Entry> sorted(first,last);
      std::sort(sorted.begin(),sorted.end());
      std::sort(expected.begin()+l,expected.begin()+r);
      assert(std::equal(sorted.begin(),sorted.end(),expected.begin()+l));
      std::copy(first,last,expected.begin()+l);
      break;
    }
    case 6:
      seq.flip(l,r);
      std::reverse(expected.begin()+l,expected.begin()+r);
      break;
    case 7:{
      auto [first,second]=seq.split(l);
      assert(seq.empty() && first.size()==l && second.size()==n-l);
      second.concat(std::move(first));
      seq=std::move(second);
      std::rotate(expected.begin(),expected.begin()+l,expected.end());
      break;
    }
    case 8:
      if constexpr(requires {seq.apply(l,r,1);}){
        const int shift=rng()%26;
        seq.apply(l,r,shift);
        for(int i=l;i<r;++i) expected[i].second=map_value(shift,expected[i].second);
      }
      break;
    case 9:
      if constexpr(requires {seq.apply_keys(l,r,Affine{},false);}){
        Affine f={descending ? -1 : 1,int(rng()%35)-17};
        seq.apply_keys(l,r,f,descending);
        for(int i=l;i<r;++i) expected[i].first=map_key(f,expected[i].first);
      }
      break;
    case 10:
      if(l<n) assert(seq.get(l)==expected[l]);
      break;
    default:
      seq.flip_all();
      std::reverse(expected.begin(),expected.end());
      break;
    }
    assert(seq.size()==(int)expected.size());
    if(q%13==0) assert(seq.to_vector()==expected);
    assert(seq.prod_all()==product(expected,0,expected.size()));
    assert(seq.prodrev_all()==product(expected,0,expected.size(),true));
  }
  assert(seq.to_vector()==expected);
}

template<class Manager> void references_and_tags(){
  Manager arena;
  auto seq=arena.new_sequence({{10,"a"},{30,"b"},{20,"c"},{40,"d"},{50,"e"}});
  auto tracked=seq.ref_at(1);
  seq.sort_inc(0,4);
  assert(seq.index_of(tracked)==2 && seq.prod_all()=="acbde");
  if constexpr(requires {seq.apply(0,5,1);}) seq.apply(0,5,1);
  if constexpr(requires {seq.apply_keys(0,5,Affine{},true);}) seq.apply_keys(0,5,{-1,100},true);
  seq.flip_all();
  auto [first,second]=seq.split(2);
  assert(first.index_of(tracked)==-1 && second.index_of(tracked)==0);
  if constexpr(requires {seq.apply_keys(0,5,Affine{},true);}) assert(second.get(0).first==70);
  if constexpr(requires {seq.apply(0,5,1);}) assert(second.get(0).second=="c");
  auto copy=second.clone();
  assert(copy.index_of(tracked)==-1 && copy.to_vector()==second.to_vector());
  Manager other;
  auto dest=other.new_sequence();
  dest.concat(std::move(second));
  assert(second.empty() && dest.index_of(tracked)==0);
  dest.set(0,{15,"z"});
  assert(dest.index_of(tracked)==0 && (dest.kth(0)==Entry{15,"z"}));
  dest.erase_at(0);
  dest.insert_at(0,15,"a");
  assert(dest.index_of(tracked)==-1);
  auto removed=dest.ref_at(0);
  dest.clear();
  dest.insert_at(0,1,"a");
  assert(dest.index_of(removed)==-1);
}

template<class Manager> void sort_with_pending_tags(){
  Manager arena;
  for(int n : {16,17,128,129,257}){
    std::vector<Entry> expected;
    for(int i = 0; i < n; ++i) expected.emplace_back(i*37%n,std::string(1,'a'+i%26));
    auto seq = arena.new_sequence(expected);
    auto tracked = seq.ref_at(n/2);
    const int tracked_key = 1000-expected[n/2].first;
    auto less = [](const Entry& a,const Entry& b){return a.first < b.first;};
    if(n == 257){
      seq.sort_inc(0,n/2);
      seq.sort_inc(n/2,n);
      std::sort(expected.begin(),expected.begin()+n/2,less);
      std::sort(expected.begin()+n/2,expected.end(),less);
    }
    seq.apply(0,n,3);
    seq.apply_keys(0,n,{-1,1000},true);
    seq.flip_all();
    for(auto& [key,value] : expected){key = 1000-key; value = map_value(3,value);}
    std::reverse(expected.begin(),expected.end());
    seq.sort_inc(0,n);
    std::sort(expected.begin(),expected.end(),less);
    auto nodes = arena.allocated_nodes();
    seq.sort_inc(2,n-2);
    assert(arena.allocated_nodes() == nodes && seq.to_vector() == expected);
    seq.sort_dec(1,n-1);
    std::reverse(expected.begin()+1,expected.end()-1);
    seq.sort_inc(2,9);
    std::sort(expected.begin()+2,expected.begin()+9,less);
    assert(seq.to_vector() == expected && seq.prodrev(0,n) == product(expected,0,n,true));
    int rank = seq.index_of(tracked);
    assert(rank >= 0 && seq.get(rank).first == tracked_key);
  }
}

struct Direction {
  bool descending;
  bool operator()(int a,int b) const {return descending ? a>b : a<b;}
};

template<class Manager> void copy_sorted_blocks(){
  Manager arena(Direction{false});
  for(bool descending : {false,true}){
    auto dest = arena.new_sequence();
    typename Manager::NodeRef tracked;
    std::vector<Entry> expected;
    {
      Manager donor(Direction{descending});
      auto seq = donor.new_sequence({{3,"a"},{2,"b"},{2,"c"},{1,"d"}});
      tracked = seq.ref_at(0);
      seq.sort_inc(0,4);
      seq.flip_all();
      expected = seq.to_vector();
      auto copy = seq.clone();
      assert(copy.to_vector() == expected && copy.index_of(tracked) == -1);
      dest.concat(std::move(seq));
      assert(seq.empty());
    }
    assert(dest.to_vector() == expected);
    dest.sort_inc(0,4);
    int rank = dest.index_of(tracked);
    assert(rank == 3 && dest.get(rank) == (Entry{3,"a"}));
    for(int i = 0; i < 3; ++i) assert(dest.get(i).first <= dest.get(i+1).first);
  }
}
}

int main(){
  using Splay = SplaySortable<int,std::string,op,identity>;
  using Treap = TreapSortable<int,std::string,op,identity,std::greater<int>>;
  using LazySplay = LazySplaySortable<int,std::string,op,identity,int,map_value,compose_value,value_id>;
  using LazyTreap = LazyTreapSortable<int,std::string,op,identity,int,map_value,compose_value,value_id>;
  using KeySplay = MonotoneSplaySortable<int,std::string,op,identity,Affine,map_key,affine_compose,affine_id>;
  using KeyTreap = MonotoneTreapSortable<int,std::string,op,identity,Affine,map_key,affine_compose,affine_id>;
  using BothSplay = LazyMonotoneSplaySortable<int,std::string,op,identity,
      Affine,map_key,affine_compose,affine_id,int,map_value,compose_value,value_id>;
  using BothTreap = LazyMonotoneTreapSortable<int,std::string,op,identity,
      Affine,map_key,affine_compose,affine_id,int,map_value,compose_value,value_id>;
  random_operations<Splay>();
  random_operations<Treap>(std::greater<int>{});
  random_operations<LazySplay>();
  random_operations<LazyTreap>();
  random_operations<KeySplay>();
  random_operations<KeyTreap>();
  random_operations<BothSplay>();
  random_operations<BothTreap>();
  references_and_tags<BothSplay>();
  references_and_tags<BothTreap>();
  sort_with_pending_tags<BothSplay>();
  sort_with_pending_tags<BothTreap>();
  copy_sorted_blocks<SplaySortable<int,std::string,op,identity,Direction>>();
  copy_sorted_blocks<TreapSortable<int,std::string,op,identity,Direction>>();
  std::cout<<"sortable_trees: all tests passed\n";
}
